#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/time.h>

#include <MIB.h>
#include <RTC.h>
#include <SSMP.h>
#include <auth.h>
#include <breakout.h>
#include <encoding/ASN.1/BER.h>
#include <encoding/bisync/bisync.h>
#include <encoding/ssmp/ssmp.h>
#include <log.h>
#include <sys.h>
#include <types/buffer.h>
#include <uart1.h>
#include <usb.h>

#define LOGTAG "SSMP"

extern bool MIB_has(const char *OID);
extern int64_t MIB_get(const char *OID, value *v);
extern int64_t MIB_set(const char *OID, const value u, value *v);

const int64_t SSMP_IDLE = 5000; // ms
const int64_t SSMP_ERROR_NONE = 0;
const int64_t SSMP_ERROR_NO_SUCH_OBJECT = 2;
const int64_t SSMP_ERROR_BAD_VALUE = 3;
const int64_t SSMP_ERROR_READONLY = 4;
const int64_t SSMP_ERROR_NO_ACCESS = 6;
const int64_t SSMP_ERROR_WRONG_TYPE = 7;
const int64_t SSMP_ERROR_COMMIT_FAILED = 14;
const int64_t SSMP_ERROR_AUTHORIZATION = 16;
const int64_t SSMP_ERROR_NOT_WRITABLE = 17;

typedef enum {
    TRAP_INPUT,
    TRAP_CARD,
    TRAP_UNKNOWN,
} trap_type;

typedef struct TRAP {
    EVENT event;
    const MIBItem *const mib;
    trap_type tag;
    union {
        bool input;
        char *card;
    };
} TRAP;

static const TRAP TRAPS[] = {
    // ... POR
    {EVENT_SYS_START, &MIB_CONTROLLER_SYSERROR_RESTART, TRAP_INPUT, {.input = true}},
    {EVENT_SYS_RESET, &MIB_CONTROLLER_SYSERROR_WATCHDOG, TRAP_INPUT, {.input = true}},

    // ... door open/close, button press/release
    {EVENT_DOOR_1_OPEN, &MIB_DOORS_1_OPEN, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_1_CLOSE, &MIB_DOORS_1_OPEN, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_1_PRESSED, &MIB_DOORS_1_BUTTON, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_1_RELEASED, &MIB_DOORS_1_BUTTON, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_1_SWIPE, &MIB_DOORS_1_SWIPE, TRAP_CARD, {.card = NULL}},

    {EVENT_DOOR_2_OPEN, &MIB_DOORS_2_OPEN, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_2_CLOSE, &MIB_DOORS_2_OPEN, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_2_PRESSED, &MIB_DOORS_2_BUTTON, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_2_RELEASED, &MIB_DOORS_2_BUTTON, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_2_SWIPE, &MIB_DOORS_2_SWIPE, TRAP_CARD, {.card = NULL}},

    {EVENT_DOOR_3_OPEN, &MIB_DOORS_3_OPEN, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_3_CLOSE, &MIB_DOORS_3_OPEN, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_3_PRESSED, &MIB_DOORS_3_BUTTON, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_3_RELEASED, &MIB_DOORS_3_BUTTON, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_3_SWIPE, &MIB_DOORS_3_SWIPE, TRAP_CARD, {.card = NULL}},

    {EVENT_DOOR_4_OPEN, &MIB_DOORS_4_OPEN, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_4_CLOSE, &MIB_DOORS_4_OPEN, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_4_PRESSED, &MIB_DOORS_4_BUTTON, TRAP_INPUT, {.input = true}},
    {EVENT_DOOR_4_RELEASED, &MIB_DOORS_4_BUTTON, TRAP_INPUT, {.input = false}},
    {EVENT_DOOR_4_SWIPE, &MIB_DOORS_4_SWIPE, TRAP_CARD, {.card = NULL}},
};

void SSMP_rxchar(uint8_t ch);
void SSMP_enq();
void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len);
void SSMP_touched();
void SSMP_get(const char *community, int64_t rqid, const char *OID);
bool SSMP_set(const char *community, int64_t rqid, const char *OID, const value val);
void SSMP_err(const char *community, int64_t rqid, const char *OID, int64_t error, int64_t index);
bool SSMP_write(const uint8_t *buffer, int N);

struct {
    circular_buffer buffer;
    bisync codec;
    bool USB;
    absolute_time_t touched;
} ssmp = {
    .buffer = {
        .head = 0,
        .tail = 0,
    },

    .codec = {
        .header = {
            .ix = 0,
            .data = {0},
        },
        .data = {
            .ix = 0,
            .data = {0},
        },
        .crc = {
            .ix = 0,
            .CRC = 0,
        },

        .DLE = false,
        .SOH = false,
        .STX = false,
        .CRC = false,

        .enq = SSMP_enq,
        .received = SSMP_received,
    },

    .USB = false,
    .touched = 0,
};

void SSMP_init() {
    if (strcasecmp(SSMP, "USB") == 0) {
        ssmp.USB = true;
    } else {
        ssmp.USB = false;
    }

    SSMP_touched();

    infof(LOGTAG, "initialised");
}

void SSMP_start() {
    debugf(LOGTAG, "start");
}

void SSMP_reset() {
    debugf(LOGTAG, "reset");

    SSMP_touched();
}

/** Updates the internal 'touched' timestamp.
 *
 */
void SSMP_touched() {
    ssmp.touched = get_absolute_time();
}

void SSMP_rx(circular_buffer *buffer) {
    buffer_flush(buffer, SSMP_rxchar);
}

void SSMP_rxchar(uint8_t ch) {
    bisync_decode(&ssmp.codec, ch);
}

void SSMP_enq() {
    debugf(LOGTAG, "ENQ");

    SSMP_touched();
    SSMP_write(SYN_SYN_ACK, 3);
}

void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len) {
    debugf(LOGTAG, "received (%d bytes)", data_len);

    // ... decode packet
    vector *fields = BER_decode(data, data_len);
    packet *request = ssmp_decode(fields);

    // ... GET request?
    if (request != NULL && request->tag == PACKET_GET) {
        debugf(LOGTAG, "GET %lld %lld %lld %s %s",
               request->get.request_id,
               request->get.error,
               request->get.error_index,
               request->community,
               request->get.OID);

        const char *community = request->community;
        const char *oid = request->get.OID;
        uint32_t rqid = request->get.request_id;

        if (!MIB_has(oid)) {
            SSMP_err(community, rqid, oid, SSMP_ERROR_NO_SUCH_OBJECT, 1);
        } else if (!auth_authorised(community, oid, OP_GET) && auth_validate(community, rqid)) {
            SSMP_err(community, rqid, oid, SSMP_ERROR_NO_ACCESS, 1);
        } else {
            SSMP_touched();
            SSMP_get(community, rqid, oid);
        }
    }

    // ... SET request?
    if (request != NULL && request->tag == PACKET_SET) {
        char val[64] = {0};

        switch (request->set.value.tag) {
        case VALUE_UINT8:
            snprintf(val, sizeof(val), "%d", request->set.value.integer);
            break;

        case VALUE_UINT16:
            snprintf(val, sizeof(val), "%d", request->set.value.integer);
            break;

        case VALUE_UINT32:
            snprintf(val, sizeof(val), "%d", request->set.value.integer);
            break;

        case VALUE_OCTET_STRING:
            snprintf(val, (request->set.value.octets.length + 1) < sizeof(val) ? sizeof(val) : request->set.value.octets.length + 1, "%s", request->set.value.octets.bytes);
            break;

        case VALUE_NULL:
            snprintf(val, sizeof(val), "NULL");
            break;

        default:
            snprintf(val, sizeof(val), "???? <%d>", request->set.value.tag);
        }

        debugf(LOGTAG, "SET %lld %lld %lld %s %s %s",
               request->get.request_id,
               request->get.error,
               request->get.error_index,
               request->community,
               request->get.OID,
               val);

        const char *community = request->community;
        const char *oid = request->get.OID;
        uint32_t rqid = request->get.request_id;

        if (!MIB_has(oid)) {
            SSMP_err(community, rqid, oid, SSMP_ERROR_NO_SUCH_OBJECT, 1);
        } else if (!auth_authorised(community, oid, OP_SET) && auth_validate(community, rqid)) {
            SSMP_err(community, rqid, oid, SSMP_ERROR_NO_ACCESS, 1);
        } else if (SSMP_set(community, rqid, oid, request->set.value)) {
            SSMP_touched();

            // FIXME check OID save list
            message msg = {
                .message = MSG_SAVE,
                .tag = MESSAGE_NONE,
            };

            push(msg);
        }
    }

    packet_free(request);
    vector_free(fields);
}

/* SSMP GET implementation.
 *
 */
void SSMP_get(const char *community, int64_t rqid, const char *OID) {
    value v;
    int64_t err = MIB_get(OID, &v);

    if (err != SSMP_ERROR_NONE) {
        SSMP_err(community, rqid, OID, err, 1);
    } else {
        packet reply = {
            .tag = PACKET_RESPONSE,
            .version = 0,
            .community = strdup(community),
            .response = {
                .request_id = rqid,
                .error = 0,
                .error_index = 0,
                .OID = strdup(OID),
                .value = v,
            },
        };

        if (v.tag == VALUE_UNKNOWN) {
            reply.response.error = SSMP_ERROR_NO_SUCH_OBJECT;
            reply.response.error_index = 1;
        }

        // ... encode
        slice packed = ssmp_encode(reply);
        slice encoded = bisync_encode(NULL, 0, packed.bytes, packed.length);

        SSMP_write(encoded.bytes, encoded.length);

        slice_free(&encoded);
        slice_free(&packed);
        packet_free(&reply);
    }
}

/* SSMP SET implementation.
 *
 */
bool SSMP_set(const char *community, int64_t rqid, const char *OID, const value val) {
    value v;
    int64_t err = MIB_set(OID, val, &v);

    packet reply = {
        .tag = PACKET_RESPONSE,
        .version = 0,
        .community = strdup(community),
        .response = {
            .request_id = rqid,
            .error = 0,
            .error_index = 0,
            .OID = strdup(OID),
            .value = v,
        },
    };

    if (err != SSMP_ERROR_NONE) {
        reply.response.error = err;
        reply.response.error_index = 1;
    }

    // ... encode
    slice packed = ssmp_encode(reply);
    slice encoded = bisync_encode(NULL, 0, packed.bytes, packed.length);

    SSMP_write(encoded.bytes, encoded.length);

    slice_free(&encoded);
    slice_free(&packed);
    packet_free(&reply);

    return err == SSMP_ERROR_NONE;
}

/* SSMP event TRAP implementation.
 *
 */
void SSMP_trap(EVENT event, void *data) {
    if (event == EVENT_UNKNOWN) {
        return;
    }

    // ... timestamp
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    char timestamp[20] = {0};

    if (RTC_get_datetime(&year, &month, &day, &hour, &minute, &second, NULL)) {
        snprintf(timestamp, sizeof(timestamp), "%04u-%02u-%02u %02u:%02u:%02u", year, month, day, hour, minute, second);
    } else {
        snprintf(timestamp, sizeof(timestamp), "---- -- --");
    }

    packet trap = {
        .tag = PACKET_TRAP,
        .version = 0,
        .community = strdup("public"),
        .trap = {
            .OID = strdup("0.1.3.6.1.4.1.65536"),
            .id = CONTROLLER,
            .category = 6, // generic-trap:  enterprise specific
            .event = 0,    // specific-trap: event type
            .timestamp = strdup(timestamp),
        },
    };

    // ... append event var(s)
    int N = sizeof(TRAPS) / sizeof(TRAP);

    for (int i = 0; i < N; i++) {
        if (event == TRAPS[i].event) {
            TRAP v = TRAPS[i];

            trap.trap.var.OID = strdup(v.mib->OID);
            switch (v.tag) {
            case TRAP_INPUT:
                trap.trap.var.value = (value){
                    .tag = VALUE_BOOLEAN,
                    .boolean = v.input,
                };
                break;

            case TRAP_CARD:
                trap.trap.var.value = (value){
                    .tag = VALUE_OCTET_STRING,
                    .octets = {
                        .length = strlen((char *)data),
                        .bytes = strdup((char *)data),
                    },
                };
                break;
            }
        }
    }

    // ... encode
    slice packed = ssmp_encode(trap);
    slice encoded = bisync_encode(NULL, 0, packed.bytes, packed.length);

    SSMP_write(encoded.bytes, encoded.length);

    slice_free(&encoded);
    slice_free(&packed);
    packet_free(&trap);
}

/* SSMP error implementation.
 *
 */
void SSMP_err(const char *community, int64_t rqid, const char *OID, int64_t error, int64_t index) {
    packet reply = {
        .tag = PACKET_RESPONSE,
        .version = 0,
        .community = strdup(community),
        .response = {
            .request_id = rqid,
            .error = error,
            .error_index = index,
            .OID = strdup(OID),
            .value = (value){.tag = VALUE_NULL},
        },
    };

    // ... encode
    slice packed = ssmp_encode(reply);
    slice encoded = bisync_encode(NULL, 0, packed.bytes, packed.length);

    SSMP_write(encoded.bytes, encoded.length);

    slice_free(&encoded);
    slice_free(&packed);
    packet_free(&reply);
}

/* SSMP write
 *
 */
bool SSMP_write(const uint8_t *buffer, int N) {
    if (ssmp.USB) {
        return usb_write(buffer, N);
    } else {
        return uart1_write(buffer, N);
    }
}
