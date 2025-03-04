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
#include <sys.h>
#include <types/buffer.h>

#define LOGTAG "SSMP"
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

const int64_t SSMP_IDLE = 5000; // ms
const int64_t SSMP_ERROR_NO_SUCH_OBJECT = 0x02;

void SSMP_rxchar(uint8_t ch);
void SSMP_enq();
void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len);
void SSMP_touched();
void SSMP_get(const char *community, int64_t rqid, const char *OID);
void on_SSMP();

extern void put_rgb(uint8_t red, uint8_t green, uint8_t blue);

const uint8_t EXPECTED[] = {48, 16, 48, 14, 6, 10, 43, 6, 1, 4, 1, 132, 128, 0, 2, 1, 5, 0};

struct {
    circular_buffer buffer;
    bisync codec;
    absolute_time_t touched;
} SSMP = {
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

    .touched = 0,
};

void SSMP_init() {
    debugf(LOGTAG, "init");

    // FIXME
    // gpio_pull_up(SSMP_TX);
    // gpio_pull_up(SSMP_RX);
    //
    // gpio_set_function(SSMP_TX, GPIO_FUNC_UART);
    // gpio_set_function(SSMP_RX, GPIO_FUNC_UART);
    //
    // uart_init(SSMP_UART, BAUD_RATE);
    // uart_set_baudrate(SSMP_UART, BAUD_RATE);
    // uart_set_format(SSMP_UART, DATA_BITS, STOP_BITS, PARITY);
    // uart_set_hw_flow(SSMP_UART, false, false);
    // uart_set_fifo_enabled(SSMP_UART, true);
    // uart_set_translate_crlf(SSMP_UART, false);

    SSMP_touched();

    infof("SSMP", "initialised");
}

// Enables UART IRQ
void SSMP_start() {
    debugf(LOGTAG, "start");

    // FIXME
    // irq_set_exclusive_handler(SSMP_IRQ, on_SSMP);
    // uart_set_irq_enables(SSMP_UART, true, false);
    // irq_set_enabled(SSMP_IRQ, true);
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

// void on_SSMP() {
//     while (uart_is_readable(SSMP_UART)) {
//         uint8_t ch = uart_getc(SSMP_UART);
//
//         buffer_push(&SSMP.buffer, ch);
//     }
//
//     circular_buffer *b = &SSMP.buffer;
//     message msg = {
//         .message = MSG_RX,
//         .tag = MESSAGE_BUFFER,
//         .buffer = &SSMP.buffer,
//     };
//
//     push(msg);
// }

void SSMP_rx(circular_buffer *buffer) {
    buffer_flush(buffer, SSMP_rxchar);
}

void SSMP_rxchar(uint8_t ch) {
    bisync_decode(&SSMP.codec, ch);
}

void SSMP_enq() {
    debugf("SSMP", "ENQ");

    SSMP_touched();
    // FIXME uart_write_blocking(SSMP_UART, SYN_SYN_ACK, 3);
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

    packet_free(request);
    vector_free(fields);
}

/* SSMP GET response
 *
 */
void SSMP_get(const char *community, int64_t rqid, const char *OID) {
    value v = MIB_get(OID);

    put_rgb(32, 0, 96);

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

    // FIXME uart_write_blocking(SSMP_UART, encoded.bytes, encoded.length);

    slice_free(&encoded);
    slice_free(&packed);
    packet_free(&reply);
}
