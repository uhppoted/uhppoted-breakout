#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <dio.h>
#include <readers.h>
#include <sys.h>
#include <uart.h>

#define UART0 uart0
#define UART1 uart1

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

void on_uart0_rx();

void dispatch(uint32_t v) {
    if ((v & MSG) == MSG_RX) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
        rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_WIO) {
        wio((uint8_t)(v & 0x0fffffff));
    }

    if ((v & MSG) == MSG_INPUTS) {
        inputs((uint8_t)(v & 0x0fffffff));
    }
}

void setup_uart() {
    uart_init(UART0, 2400);

    gpio_set_function(UART0_TX, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX, GPIO_FUNC_UART);

    uart_set_baudrate(UART0, BAUD_RATE);
    uart_set_hw_flow(UART0, false, false);
    uart_set_format(UART0, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART0, false);

    irq_set_exclusive_handler(UART0_IRQ, on_uart0_rx);
    irq_set_enabled(UART0_IRQ, true);

    uart_set_irq_enables(UART0, true, false);
}

void on_uart0_rx() {
    char buffer[32];
    int ix = 0;

    while (uart_is_readable(UART0) && ix < sizeof(buffer)) {
        buffer[ix++] = uart_getc(UART0);
    }

    if (ix > 0) {
        char *b;

        if ((b = calloc(ix + 1, 1)) != NULL) {
            memmove(b, buffer, ix);
            uint32_t msg = MSG_RX | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
            if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                free(b);
            }
        }

        ix = 0;
    }
}

void blink() {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
#endif
}
