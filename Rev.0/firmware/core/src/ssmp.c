#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <log.h>
#include <ssmp.h>
#include <state.h>

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

const char SOH = 1;
const char STX = 2;
const char ETX = 3;
const char ENQ = 5;
const char ACK = 6;
const char SYN = 22;

void on_ssmp();

struct {
    char buffer[64];
    int ix;
} SSMP = {
    .buffer = {0},
    .ix = 0,
};

void ssmp_ack();

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

    infof("SSMP", "initialised");
}

// Enables interrupt handler.
void ssmp_start() {
    debugf("SSMP", "start");

    // irq_set_exclusive_handler(UART0_IRQ, on_smp);
    // irq_set_enabled(UART0_IRQ, true);
    // uart_set_irq_enables(uart0, true, false);
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

void ssmp_rx(const char *received) {
    int N = strlen(received);
    for (int i = 0; i < N; i++) {
        char ch = received[i];

        // SYN?
        if (ch == SYN) {
            memset(SSMP.buffer, 0, sizeof(SSMP.buffer));
            SSMP.ix = 0;
            continue;
        }

        // ENQ?
        if (ch == ENQ && SSMP.ix == 0) {
            memset(SSMP.buffer, 0, sizeof(SSMP.buffer));
            SSMP.ix = 0;
            ssmp_ack();
            continue;
        }
    }
}

void ssmp_ack() {
    char reply[] = {SYN, SYN, ACK};

    fwrite(reply, 1, 3, stdout);
    fflush(stdout);
}

// void ssmp_debug() {
//     bool enabled = uart_is_enabled(uart0);
//     bool writeable = uart_is_writable(uart0);
//     bool readable = uart_is_readable(uart0);
//     // uart_get_hw(uart)->cr = UART_UARTCR_UARTEN_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_RXE_BITS;
//
//     debugf("SMP", "pins: TX:%d  RX:%d", UART0_TX, UART0_RX);
//     debugf("SMP", "UART0: enabled:%s read:%s write:%s",
//            enabled ? "Y" : "N",
//            readable ? "Y" : "N",
//            writeable ? "Y" : "N");
//
//     if (writeable) {
//         uart_putc(uart0, 'X');
//         // const char *s = "qwerty";
//         //
//         // uart_write_blocking (uart0, s, 6);
//     }
// }