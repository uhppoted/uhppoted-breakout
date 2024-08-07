#include <stdio.h>

#include <pico/stdlib.h>

#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <state.h>
#include <sys.h>
#include <uart.h>

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

extern const char *VERSION;
extern void sysinit();

bool on_tick(repeating_timer_t *);

struct {
    bool reboot;
    struct repeating_timer timer;
} sys = {
    .reboot = false,
};

bool sys_init() {
    // ... SYS LED
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    if (!add_repeating_timer_ms(1000, on_tick, NULL, &sys.timer)) {
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

    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);

    uart_set_irq_enables(uart0, true, false);

    // ... system stuff
    char s[64];

    snprintf(s, sizeof(s), "-----  BREAKOUT   %s", VERSION);

    sysinit();
    cli_init();
    println(s);

    // ... other stuff
    log_init();

    return true;
}

bool on_tick(repeating_timer_t *t) {
    uint32_t msg = MSG_TICK;
    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SYS", "tick: queue full");
    }

    return true;
}

/* Blinks SYSLED and resets watchdog.
 *
 */
void sys_tick() {
    bool on = gpio_get(PICO_DEFAULT_LED_PIN);

    gpio_put(PICO_DEFAULT_LED_PIN, !on);

    if (!sys.reboot) {
        uint32_t msg = MSG_WATCHDOG;
        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "SYS", "watchdog: queue full");
        }
    }
}

/* Sets sys.reboot flag to inhibit watchdog reset.
 *
 */
void sys_reboot() {
    sys.reboot = true;
}
