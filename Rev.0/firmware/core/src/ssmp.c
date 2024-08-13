#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include <breakout.h>
#include <encoding/bisync/bisync.h>
#include <log.h>
#include <ssmp.h>
#include <state.h>
#include <sys.h>

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// clang-format off
const uint8_t RESPONSE[] = {
    48,  41,  2,   1,   0,  4,   6,  112, 
    117, 98,  108, 105, 99, 162, 28, 2, 
    1,   1,   2,   1,   0,  2,   1,  0,
    48,  17,  48,  15,  6,  7,   43, 6,
    167, 254, 32,  1,   1,  71,  4,  24, 
    42,  55,  120};
// clang-format on

const int64_t SSMP_IDLE = 5000; // ms

void ssmp_enq();
void ssmp_get(const uint8_t *header, int header_len, const uint8_t *data, int data_len);
void ssmp_echo(const uint8_t *buffer, int N);
void on_ssmp();

struct {
    struct bisync codec;
    absolute_time_t touched;
} SSMP = {
    .codec = {
        .header = {0},
        .data = {0},
        .hx = 0,
        .ix = 0,
        .DLE = false,
        .SOH = false,
        .STX = false,
        .enq = ssmp_enq,
        .received = ssmp_get,
    },
    .touched = 0,
};

void ssmp_init() {
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

    SSMP.touched = get_absolute_time();

    infof("SSMP", "initialised");
}

// Enables interrupt handler.
void ssmp_start() {
    debugf("SSMP", "start");

    // irq_set_exclusive_handler(UART0_IRQ, on_smp);
    // irq_set_enabled(UART0_IRQ, true);
    // uart_set_irq_enables(uart0, true, false);
}

void ssmp_ping() {
    if (get_mode() == MODE_SSMP) {
        absolute_time_t now = get_absolute_time();
        int64_t delta = absolute_time_diff_us(SSMP.touched, now) / 1000;

        // char s[64];
        // snprintf(s, sizeof(s), "%lld   %lld   %lld %s", SSMP.touched, now / 1000, delta, llabs(delta) > SSMP_IDLE ? "idle" : " busy");
        // printf(s);

        if (llabs(delta) > SSMP_IDLE) {
            set_mode(MODE_UNKNOWN);
        }
    }
}

void on_ssmp() {
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

    //     ix = 0;
    // }
}

void ssmp_rx(const struct buffer *received) {
    bisync_decode(&SSMP.codec, received->data, received->N);
}

void ssmp_enq() {
    SSMP.touched = get_absolute_time();

    const char *reply = SYN_SYN_ACK;

    fflush(stdout);
    fwrite(reply, 1, 3, stdout);
    fflush(stdout);
}

void ssmp_get(const uint8_t *header, int header_len, const uint8_t *data, int data_len) {
    SSMP.touched = get_absolute_time();

    message msg = bisync_encode(NULL, 0, RESPONSE, sizeof(RESPONSE));

    fwrite(msg.data, sizeof(uint8_t), msg.N, stdout);
    fflush(stdout);
    free(msg.data);
}

void ssmp_echo(const uint8_t *buffer, int N) {
    // uint8_t *reply = (char *)calloc(N + 9, sizeof(uint8_t));
    uint8_t *reply = (char *)calloc(N + 2, sizeof(uint8_t));
    uint8_t *p = reply;

    *p++ = SYN;
    *p++ = SYN;
    // *p++ = SOH;
    // *p++ = (N >> 24) & 0xff;
    // *p++ = (N >> 16) & 0xff;
    // *p++ = (N >> 8) & 0xff;
    // *p++ = (N >> 0) & 0xff;
    // *p++ = STX;

    for (int i = 0; i < N; i++) {
        *p++ = buffer[i];
    }

    // *p++ = ETX;

    fwrite(reply, 1, N + 2, stdout);
    fflush(stdout);
    free(reply);
}
