#include <stdio.h>
#include <stdlib.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <cli.h>
#include <dio.h>
#include <readers.h>
#include <sys.h>
#include <uart.h>

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

void on_uart0_rx();

void sysinit() {
    uart_init(uart0, 2400);

    gpio_set_function(UART0_TX, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX, GPIO_FUNC_UART);

    uart_set_baudrate(uart0, BAUD_RATE);
    uart_set_hw_flow(uart0, false, false);
    uart_set_format(uart0, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(uart0, false);

    irq_set_exclusive_handler(UART0_IRQ, on_uart0_rx);
    irq_set_enabled(UART0_IRQ, true);

    uart_set_irq_enables(uart0, true, false);
}

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

void blink() {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    for (int i = 0; i < 4; i++) {
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        gpio_put(LED_PIN, 1);
    }
#endif
}
