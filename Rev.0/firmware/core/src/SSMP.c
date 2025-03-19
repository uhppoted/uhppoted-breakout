#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/time.h>

#include <MIB.h>
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

const int64_t SSMP_IDLE = 5000; // ms
const int64_t SSMP_ERROR_NO_SUCH_OBJECT = 0x02;

void SSMP_rxchar(uint8_t ch);
void SSMP_enq();
void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len);
void SSMP_touched();
void SSMP_get(const char *community, int64_t rqid, const char *OID);
void SSMP_write(const uint8_t *buffer, int N);

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

    infof("SSMP", "initialised");
}

void SSMP_start() {
    debugf(LOGTAG, "start");
}

void SSMP_reset() {
    debugf("SSMP", "reset");

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
    debugf("SSMP", "ENQ");

    SSMP_touched();
    SSMP_write(SYN_SYN_ACK, 3);
}

void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len) {
    debugf("SSMP", "received (%d bytes)", data_len);

    // ... decode packet
    vector *fields = BER_decode(data, data_len);
    packet *request = ssmp_decode(fields);

    // ... GET request?
    if (request != NULL && request->tag == PACKET_GET) {
        debugf("SSMP", "GET %lld %lld %lld %s %s",
               request->get.request_id,
               request->get.error,
               request->get.error_index,
               request->community,
               request->get.OID);

        const char *community = request->community;
        const char *oid = request->get.OID;
        uint32_t rqid = request->get.request_id;

        if (auth_authorised(community, oid)) {
            if (auth_validate(community, rqid)) {
                SSMP_touched();
                SSMP_get(community, rqid, oid);
            }
        }
    }

    // ... SET request?
    if (request != NULL && request->tag == PACKET_SET) {
        char val[64] = {0};

        switch (request->set.value.tag) {
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

        debugf("SSMP", "SET %lld %lld %lld %s %s %s",
               request->get.request_id,
               request->get.error,
               request->get.error_index,
               request->community,
               request->get.OID,
               val);

        // const char *community = request->community;
        // const char *oid = request->get.OID;
        // uint32_t rqid = request->get.request_id;
        //
        // if (auth_authorised(community, oid)) {
        //     if (auth_validate(community, rqid)) {
        //         SSMP_touched();
        //         SSMP_get(community, rqid, oid);
        //     }
        // }
    }

    packet_free(request);
    vector_free(fields);
}

/* SSMP GET response
 *
 */
void SSMP_get(const char *community, int64_t rqid, const char *OID) {
    value v = MIB_get(OID);

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

/* SSMP write
 *
 */
void SSMP_write(const uint8_t *buffer, int N) {
    if (ssmp.USB) {
        usb_write(buffer, N);
    } else {
        uart1_write(buffer, N);
    }
}
