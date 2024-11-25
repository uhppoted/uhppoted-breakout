#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
bool on_monitor(repeating_timer_t *);
void put_rgb(uint8_t red, uint8_t green, uint8_t blue);
uint32_t get_total_heap();
uint32_t get_free_heap();
uint32_t counter = 0;

struct {
    bool LED;
    bool reboot;
    repeating_timer_t timer;

    alarm_pool_t *pool;
    repeating_timer_t monitor;
    absolute_time_t touched;
    bool triggered;
} sys = {
    .LED = false,
    .reboot = false,
    .touched = 0,
    .triggered = false,
};

bool sys_init() {
    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);
    put_rgb(128, 0, 0);

    // ... system tick
    if (!add_repeating_timer_ms(1000, on_tick, &sys, &sys.timer)) {
        return false;
    }

    // ... system stuff
    char s[64];

    if (!strcmp(WATCHDOG, "disabled") != 0) {
        snprintf(s, sizeof(s), "-----  BREAKOUT   v%02x.%02x   <<< NO WATCHDOG >>>", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    } else {
        snprintf(s, sizeof(s), "-----  BREAKOUT   v%02x.%02x", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    }

    sysinit();
    cli_init();
    // usb_init();
    log_init();

    println(s);

    // ... set up health watchdog
    uint t0 = TIMER_ALARM_NUM_FROM_IRQ(TIMER_IRQ_0);

    if (!hardware_alarm_is_claimed(t0)) {
        sys.pool = alarm_pool_create(t0, 1);
        sys.touched = get_absolute_time();

        alarm_pool_add_repeating_timer_ms(sys.pool, 500, on_monitor, &sys, &sys.monitor);
    }

    return true;
}

bool on_tick(repeating_timer_t *t) {
    uint32_t msg = MSG_TICK;
    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SYS", "tick: queue full");
    }

    return true;
}

/* Internal system monitor, hooked to TIMER 0 IRQ
 *
 */
bool on_monitor(repeating_timer_t *t) {
    absolute_time_t now = get_absolute_time();
    int64_t delta = absolute_time_diff_us(sys.touched, now) / 1000;

    if (llabs(delta) > 5000) {
        if (!sys.triggered) {
            sys.triggered = true;
            put_rgb(128, 0, 128);

            uint32_t heap = get_total_heap();
            uint32_t available = get_free_heap();
            float used = 1.0 - ((float)available / (float)heap);
            char *watchdogged = get_error(ERR_WATCHDOG) ? "** watchdog **" : "";

            debugf("*****", "%-5u queue:%u  total heap:%u  free heap:%u  used:%.1f%%  errors:%04x %s",
                   counter,
                   queue_get_level(&queue),
                   heap,
                   available,
                   100.0f * used,
                   get_errors(),
                   watchdogged);

            debugf("*****", "%-5u now:%ld  touched:%ld  delta:%ld",
                   counter,
                   now,
                   sys.touched,
                   delta);

            if (get_error(ERR_STDOUT)) {
                debugf("*****", "**** STDOUT");
            }
        }
    } else {
        sys.triggered = false;
    }
}

/* Blinks SYSLED and resets watchdog.
 *
 */
void sys_tick() {
    sys.LED = !sys.LED;

    if (!sys.triggered) {
        if (sys.LED && get_mode() == MODE_CLI) {
            put_rgb(8, 4, 0); // yellow'ish
        } else if (sys.LED) {
            put_rgb(0, 0, 0); // off
        } else {
            put_rgb(0, 8, 0);
        }
    }

    if (!sys.reboot) {
        uint32_t msg = MSG_WATCHDOG;
        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "SYS", "watchdog: queue full");
        }
    }

    uint32_t heap = get_total_heap();
    uint32_t available = get_free_heap();
    float used = 1.0 - ((float)available / (float)heap);
    char *watchdogged = get_error(ERR_WATCHDOG) ? "** watchdog **" : "";

    debugf("SYS", "%-5u queue:%u  total heap:%u  free heap:%u  used:%.1f%%  errors:%04x  %s",
           counter,
           queue_get_level(&queue),
           heap,
           available,
           100.0f * used,
           get_errors(),
           watchdogged);

    if (get_error(ERR_STDOUT)) {
        debugf("SYS", "**** STDOUT");
    }

    counter++;
}

/* Sets sys.reboot flag to inhibit watchdog reset.
 *
 */
void sys_reboot() {
    sys.reboot = true;
}

/* Resets the internal soft watchdog.
 *
 */
void sys_watchdog_update() {
    sys.touched = get_absolute_time();
}

void sys_debug() {
    absolute_time_t now = get_absolute_time();
    int64_t delta = absolute_time_diff_us(sys.touched, now) / 1000;

    put_rgb(0, 64, 255);

    uint32_t heap = get_total_heap();
    uint32_t available = get_free_heap();
    float used = 1.0 - ((float)available / (float)heap);
    char *watchdogged = get_error(ERR_WATCHDOG) ? "** watchdog **" : "";

    debugf("POKE", "%-5u queue:%u  total heap:%u  free heap:%u  used:%.1f%%  errors:%04x  %s",
           counter,
           queue_get_level(&queue),
           heap,
           available,
           100.0f * used,
           get_errors(),
           watchdogged);

    debugf("POKE", "%-5u now:%ld  touched:%ld  delta:%ld",
           counter,
           now,
           sys.touched,
           delta);

    if (get_error(ERR_STDOUT)) {
        debugf("POKE", "**** STDOUT");
    }
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
