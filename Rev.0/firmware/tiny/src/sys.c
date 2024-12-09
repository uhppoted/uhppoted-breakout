#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <hardware/pio.h>
#include <hardware/ticks.h>
#include <hardware/timer.h>

#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <state.h>
#include <sys.h>

#include "ws2812.pio.h"

extern void sysinit();

void put_rgb(uint8_t red, uint8_t green, uint8_t blue);
uint32_t get_total_heap();
uint32_t get_free_heap();
uint32_t counter = 0;

struct {
    struct {
        bool LED;
        mutex_t guard;
    } LED;

    bool reboot;
    repeating_timer_t timer;

    absolute_time_t touched;
} sys = {
    .LED = {
        .LED = false,
    },
    .reboot = false,
    .touched = 0,
};

bool sys_init() {
    mutex_init(&sys.LED.guard);

    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);
    put_rgb(128, 12, 0);

    // ... system tick
    if (!add_repeating_timer_ms(1000, sys_on_tick, &sys, &sys.timer)) {
        return false;
    }

    // ... system stuff
    char s[64];

    if (!strcmp(WATCHDOG, "disabled") != 0) {
        snprintf(s, sizeof(s), "-----  BREAKOUT   v%02x.%02x   <<< NO WATCHDOG >>>\n", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    } else {
        snprintf(s, sizeof(s), "-----  BREAKOUT   v%02x.%02x\n", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    }

    sysinit();
    cli_init();
    log_init();
    print(s);

    return true;
}

/* Blinks SYSLED and resets watchdog.
 *
 */
void sys_tick() {
    uint16_t errors = get_errors();

    sys.LED.LED = !sys.LED.LED;

    if (sys.LED.LED && errors != 0x0000) {
        put_rgb(96, 0, 0);
    } else if (sys.LED.LED) {
        put_rgb(0, 8, 0);
    } else {
        switch (get_mode()) {
        case MODE_NONE:
            put_rgb(0, 0, 0); // off
            break;

        case MODE_LOG:
            put_rgb(4, 4, 128); // blue
            break;

        case MODE_CLI:
            put_rgb(0, 48, 96); // turquoise
            break;

        case MODE_UNKNOWN:
            put_rgb(64, 16, 0); // yellow
            break;

        default:
            put_rgb(32, 0, 96); // purple
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
    const char *watchdogged = get_error(ERR_WATCHDOG) ? "** watchdog **" : "";

    debugf("SYS", "%-5u queue:%u  total heap:%u  free heap:%u  used:%.1f%%  errors:%04x  %s",
           counter,
           queue_get_level(&queue),
           heap,
           available,
           100.0f * used,
           errors,
           watchdogged);

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
    bool watchdog_tick = tick_is_running(TICK_WATCHDOG);
    uint64_t timer_time = time_us_64();

    printf(">> now: %llu\n", now);
    printf(">> watchdog tick: %s\n", watchdog_tick ? "ok" : "stopped");
    printf(">> timer time (us): %llu\n", timer_time);
    printf("-------\n");
}

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    if (mutex_try_enter(&sys.LED.guard, NULL)) {
        uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

        pio_sm_put_blocking(pio0, 0, rgb << 8u);
        mutex_exit(&sys.LED.guard);
    }
}
