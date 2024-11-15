#include <malloc.h>
#include <stdio.h>

#include <pico/stdlib.h>

#include <hardware/pio.h>

#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <state.h>
#include <sys.h>
#include <usb.h>

#include "ws2812.pio.h"

extern void sysinit();

bool on_tick(repeating_timer_t *);
void put_rgb(uint8_t red, uint8_t green, uint8_t blue);
uint32_t get_total_heap();
uint32_t get_free_heap();
uint32_t counter = 0;

struct {
    bool LED;
    bool reboot;
    struct repeating_timer timer;
} sys = {
    .LED = false,
    .reboot = false,
};

bool sys_init() {
    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);
    put_rgb(128, 0, 0);

    if (!add_repeating_timer_ms(1000, on_tick, &sys, &sys.timer)) {
        return false;
    }

    // ... system stuff
    char s[64];

    snprintf(s, sizeof(s), "-----  BREAKOUT   v%02x.%02x", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);

    sysinit();
    cli_init();
    println(s);

    // ... other stuff
    usb_init();
    log_init();

    return true;
}

bool on_tick(repeating_timer_t *t) {
    uint32_t msg = MSG_TICK;
    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SYS", "blink: queue full");
    }

    return true;
}

/* Blinks SYSLED and resets watchdog.
 *
 */
void sys_tick() {
    sys.LED = !sys.LED;

    if (sys.LED && get_mode() == MODE_CLI) {
        put_rgb(8, 4, 0); // yellow'ish
    } else if (sys.LED) {
        put_rgb(0, 0, 0); // off
    } else {
        put_rgb(0, 8, 0);
    }

    if (!sys.reboot) {
        uint32_t msg = MSG_WATCHDOG;
        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "SYS", "watchdog: queue full");
        }
    }

    debugf("SYS", "%-5u queue:%u  total heap:%u  free heap:%u", counter++, queue_get_level(&queue), get_total_heap(), get_free_heap());
}

/* Sets sys.reboot flag to inhibit watchdog reset.
 *
 */
void sys_reboot() {
    sys.reboot = true;
}

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

    pio_sm_put_blocking(pio0, 0, rgb << 8u);
}

uint32_t get_total_heap() {
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

uint32_t get_free_heap() {
    extern char __StackLimit, __bss_end__;

    uint32_t total = &__StackLimit - &__bss_end__;
    struct mallinfo m = mallinfo();

    return total - m.uordblks;
}