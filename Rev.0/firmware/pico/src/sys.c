#include <stdio.h>
#include <stdlib.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <cli.h>
#include <dio.h>
#include <log.h>
#include <readers.h>
#include <state.h>
#include <sys.h>
#include <uart.h>

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

void on_uart0_rx();
bool blink(repeating_timer_t *);

struct {
    struct repeating_timer timer;
} sysled;

bool sysinit() {
    // ... SYS LED
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    if (!add_repeating_timer_ms(1000, blink, NULL, &sysled.timer)) {
        return false;
    }
#endif

    // ... UART
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

    return true;
}

void dispatch(uint32_t v) {
    if ((v & MSG) == MSG_RX) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
        rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_WIO) {
        uint8_t io = v & 0x000000ff;
        uint8_t mask = (v >> 8) & 0x000000ff;

        wio(io, mask);
    }

    if ((v & MSG) == MSG_INPUTS) {
        uint8_t io = v & 0x000000ff;
        uint8_t mask = (v >> 8) & 0x000000ff;

        inputs(io, mask);
    }

    if ((v & MSG) == MSG_TICK) {
        infof("SYS", "tick");
    }
}

/* SYSLED blink callback.
 *
 */
bool blink(repeating_timer_t *t) {
    bool on = gpio_get(PICO_DEFAULT_LED_PIN);

    gpio_put(PICO_DEFAULT_LED_PIN, !on);

    uint32_t msg = MSG_TICK | ((uint32_t)inputs & 0x00000000);
    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SYS", "blink: queue full");
    }

    return true;
}
