#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include <MIB.h>
#include <SSMP.h>
#include <auth.h>
#include <auth/hotp/hotp.h>
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

    // // ... UART
    // gpio_set_function(UART0_TX, GPIO_FUNC_UART);
    // gpio_set_function(UART0_RX, GPIO_FUNC_UART);
    //
    // uart_init(uart0, BAUD_RATE);
    // uart_set_baudrate(uart0, BAUD_RATE);
    // uart_set_format(uart0, DATA_BITS, STOP_BITS, PARITY);
    // uart_set_hw_flow(uart0, false, false);
    // uart_set_fifo_enabled(uart0, false);

    SSMP_touched();

    infof("SSMP", "initialised");
}

// Enables interrupt handler.
void SSMP_start() {
    debugf("SSMP", "start");

    // irq_set_exclusive_handler(UART0_IRQ, on_smp);
    // irq_set_enabled(UART0_IRQ, true);
    // uart_set_irq_enables(uart0, true, false);
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
    if (get_mode() == MODE_SSMP) {
        absolute_time_t now = get_absolute_time();
        int64_t delta = absolute_time_diff_us(SSMP.touched, now) / 1000;

        if (llabs(delta) > SSMP_IDLE) {
            set_mode(MODE_UNKNOWN);
        }
    }
}

void on_SSMP() {
    // char buffer[32];
    // int ix = 0;
    //
    // while (uart_is_readable(uart0) && ix < sizeof(buffer)) {
    //     buffer[ix++] = uart_getc(uart0);
    // }
    //
    // if (ix > 0) {
    //     char *b;
    //
    //     // FIXME use struct with length
    //     if ((b = calloc(ix + 1, 1)) != NULL) {
    //         memmove(b, buffer, ix);
    //         uint32_t msg = MSG_RX | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
    //         if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
    //             set_error(ERR_QUEUE_FULL, "SMP", "rx: queue full");
    //             free(b);
    //         }
    //     }
    //
    //     ix = 0;
    // }
}

void SSMP_rx(const struct buffer *received) {
    bisync_decode(&SSMP.codec, received->data, received->N);
}

void SSMP_enq() {
    SSMP_touched();

    const char *reply = SYN_SYN_ACK;

    fflush(stdout);
    fwrite(reply, 1, 3, stdout);
    fflush(stdout);
}

void SSMP_received(const uint8_t *header, int header_len, const uint8_t *data, int data_len) {
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

        if (authorised(community, oid)) {
            if (hotp_validate(community, code)) {
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
    value v = MIB_get(OID);

    struct packet reply = {
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

    // ... encode
    slice packed = ssmp_encode(reply);
    slice encoded = bisync_encode(NULL, 0, packed.bytes, packed.length);

    fwrite(encoded.bytes, sizeof(uint8_t), encoded.length, stdout);
    fflush(stdout);

    slice_free(&packed);
    slice_free(&encoded);
    free_packet(&reply);
}
