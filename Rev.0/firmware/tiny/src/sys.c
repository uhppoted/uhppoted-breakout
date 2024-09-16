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

extern const char *VERSION;
extern void sysinit();

bool on_tick(repeating_timer_t *);
void put_rgb(uint8_t red, uint8_t green, uint8_t blue);

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

    snprintf(s, sizeof(s), "-----  BREAKOUT   %s", VERSION);

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

    if (sys.LED) {
        switch (get_mode()) {
        case MODE_CLI:
            put_rgb(6, 4, 0); // yellow'ish
            break;

        case MODE_SSMP:
            put_rgb(0, 0, 96); // blue
            break;

        default:
            put_rgb(0, 0, 0); // off
        }
    } else {
        put_rgb(0, 8, 0);
    }

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

/* Enables/disables LF to CRLF translation on USB.
 *
 */
void sys_translate_crlf(bool enabled) {
    stdio_set_translate_crlf(&stdio_usb, enabled);
}

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

    pio_sm_put_blocking(pio0, 0, rgb << 8u);
}
