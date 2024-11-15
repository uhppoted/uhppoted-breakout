#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include <MIB.h>
#include <SSMP.h>
#include <auth.h>
#include <breakout.h>
#include <encoding/ASN.1/BER.h>
#include <encoding/bisync/bisync.h>
#include <encoding/ssmp/ssmp.h>
#include <log.h>
#include <state.h>
#include <sys.h>

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

const int64_t SSMP_IDLE = 5000; // ms
const int64_t SSMP_ERROR_NO_SUCH_OBJECT = 0x02;

void SSMP_enq();
void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len);
void SSMP_touched();
void SSMP_get(const char *community, int64_t rqid, const char *OID);
void on_SSMP();

struct {
    struct bisync codec;
    absolute_time_t touched;
} SSMP = {
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
    .touched = 0,
};

void SSMP_init() {
    debugf("SSMP", "init");

    // ... UART
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);

    uart_init(UART, BAUD_RATE);
    uart_set_baudrate(UART, BAUD_RATE);
    uart_set_format(UART, DATA_BITS, STOP_BITS, PARITY);
    uart_set_hw_flow(UART, false, false);
    uart_set_fifo_enabled(UART, true);
    uart_set_translate_crlf(UART, false);

    SSMP_touched();

    infof("SSMP", "initialised");
}

// Enables interrupt handler.
void SSMP_start() {
    debugf("SSMP", "start");

    irq_set_exclusive_handler(UART_IRQ, on_SSMP);
    uart_set_irq_enables(UART, true, false);
    // irq_set_enabled(UART_IRQ, true);
    irq_set_enabled(UART_IRQ, false);
}

void SSMP_reset() {
    debugf("SSMP", "reset");

    SSMP_touched();
}

/** Updates the internal 'touched' timestamp.
 *
 */
void SSMP_touched() {
    SSMP.touched = get_absolute_time();
}

void SSMP_ping() {
    debugf("SSMP", "ping");

    //     absolute_time_t now = get_absolute_time();
    //     int64_t delta = absolute_time_diff_us(SSMP.touched, now) / 1000;
    //
    //     if (llabs(delta) > SSMP_IDLE) {
    //     }
}

void on_SSMP() {
    char buf[32];
    size_t ix = 0;

    // FIXME bool uart_is_readable_within_us (uart_inst_t * uart, uint32_t us)
    while (uart_is_readable(UART) && ix < sizeof(buf)) {
        buf[ix++] = uart_getc(UART);
    }

    // debugf("SSMP", "RX %d", ix);
    //
    // if (ix > 0) {
    // struct buffer *b;
    //
    // if ((b = (struct buffer *)malloc(sizeof(struct buffer))) != NULL) {
    //     b->N = ix;
    //     memmove(b->data, buf, ix);
    //
    //     uint32_t msg = MSG_RX | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
    //     if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
    //         set_error(ERR_QUEUE_FULL, "SSMP", "rx: queue full");
    //         free(b);
    //     }
    // }
    //
    // ix = 0;
    // }
}

void SSMP_rx(const struct buffer *received) {
    debugf("SSMP", ">> RX %d", received->N);
    bisync_decode(&SSMP.codec, received->data, received->N);
}

void SSMP_enq() {
    debugf("SSMP", "ENQ");
    SSMP_touched();
    uart_write_blocking(UART, SYN_SYN_ACK, 3);
}

void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len) {
    debugf("SSMP", "received");

    // ... decode packet
    vector *fields = BER_decode(data, data_len);
    packet *request = ssmp_decode(fields);

    vector_free(fields);
    free(fields);

    // ... GET request?
    if (request != NULL && request->tag == PACKET_GET) {
        const char *community = request->community;
        const char *oid = request->get.OID;
        uint32_t code = request->get.request_id;

        if (auth_authorised(community, oid)) {
            if (auth_validate(community, code)) {
                SSMP_touched();
                SSMP_get(request->community, request->get.request_id, request->get.OID);
            }
        }
    }

    free_packet(request);
}

/* SSMP GET response
 *
 */
void SSMP_get(const char *community, int64_t rqid, const char *OID) {
    debugf("SSMP", "GET");
    value v = MIB_get(OID);

    packet reply = {
        .tag = PACKET_GET_RESPONSE,
        .version = 0,
        .community = strdup(community),
        .get_response = {
            .request_id = rqid,
            .error = 0,
            .error_index = 0,
            .OID = strdup(OID),
            .value = v,
        },
    };

    if (v.tag == VALUE_UNKNOWN) {
        reply.get_response.error = SSMP_ERROR_NO_SUCH_OBJECT;
        reply.get_response.error_index = 1;
    }

    // ... encode
    slice packed = ssmp_encode(reply);
    slice encoded = bisync_encode(NULL, 0, packed.bytes, packed.length);

    debugf("SSMP", "GET/response %u", encoded.length);
    uart_write_blocking(UART, encoded.bytes, encoded.length);

    slice_free(&encoded);
    slice_free(&packed);
    free_packet(&reply);
}
