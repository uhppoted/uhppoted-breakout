#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <log.h>
#include <state.h>
#include <txrx.h>

void on_txrx();

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

void TXRX_init() {
    debugf("TXRX", "init");

    // ... UART
    gpio_set_function(UART0_TX, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX, GPIO_FUNC_UART);

    uart_init(uart0, BAUD_RATE);
    uart_set_baudrate(uart0, BAUD_RATE);
    uart_set_format(uart0, DATA_BITS, STOP_BITS, PARITY);
    uart_set_hw_flow(uart0, false, false);
    uart_set_fifo_enabled(uart0, false);

    infof("TXRX", "initialised");
}

// Enables interrupt handler.
void TXRX_start() {
    debugf("TXRX", "start");

    irq_set_exclusive_handler(UART0_IRQ, on_txrx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);
}

void on_txrx() {
    char buffer[32];
    int ix = 0;

    while (uart_is_readable(uart0) && ix < sizeof(buffer)) {
        buffer[ix++] = uart_getc(uart0);
    }

    if (ix > 0) {
        char *b;

        // FIXME use struct with length
        if ((b = calloc(ix + 1, 1)) != NULL) {
            memmove(b, buffer, ix);
            uint32_t msg = MSG_RX | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
            if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                set_error(ERR_QUEUE_FULL, "TXRX", "rx: queue full");
                free(b);
            }
        }

        ix = 0;
    }
}

void TXRX_rx(const char *buffer) {
    debugf("TXRX", "RX: %s", buffer);
}

void TXRX_debug() {
    bool enabled = uart_is_enabled(uart0);
    bool writeable = uart_is_writable(uart0);
    bool readable = uart_is_readable(uart0);
    // uart_get_hw(uart)->cr = UART_UARTCR_UARTEN_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_RXE_BITS;

    debugf("TXRX", "pins: TX:%d  RX:%d", UART0_TX, UART0_RX);
    debugf("TXRX", "UART0: enabled:%s read:%s write:%s",
           enabled ? "Y" : "N",
           readable ? "Y" : "N",
           writeable ? "Y" : "N");

    if (writeable) {
        uart_putc(uart0, 'X');
        // const char *s = "qwerty";
        //
        // uart_write_blocking (uart0, s, 6);
    }
}