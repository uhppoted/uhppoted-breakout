#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <hardware/pio.h>
#include <hardware/ticks.h>

#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <sys.h>
#include <types/buffer.h>
#include <usb.h>

#include "ws2812.pio.h"

extern bool sysinit();

void put_rgb(uint8_t red, uint8_t green, uint8_t blue);

struct {
    struct {
        bool LED;
        mutex_t guard;
    } LED;

} sys = {
    .LED = {
        .LED = false,
    },
};

bool sys_init() {
    mutex_init(&sys.LED.guard);

    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);
    put_rgb(128, 12, 0);

    // ... initialise
    if (!sysinit()) {
        return false;
    }

    if (!usb_init()) {
        return false;
    }

    log_init();
    cli_init();

    // ... startup message
    char s[64];

    if (!strcmp(WATCHDOG, "disabled") != 0) {
        snprintf(s, sizeof(s), "-----  BREAKOUT   v%02x.%02x   <<< NO WATCHDOG >>>\n", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    } else {
        snprintf(s, sizeof(s), "-----  BREAKOUT   v%02x.%02x\n", (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);
    }

    print(s);

    return true;
}

/* Blinks SYSLED.
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
            put_rgb(64, 16, 0); // yellow
            break;

        case MODE_UNKNOWN:
            put_rgb(0, 48, 96); // turquoise
            break;

        default:
            put_rgb(32, 0, 96); // purple
        }
    }
}

// NTS: mutex because the LED was occasionally used for debugging
void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    if (mutex_try_enter(&sys.LED.guard, NULL)) {
        uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

        pio_sm_put_blocking(pio0, 0, rgb << 8u);
        mutex_exit(&sys.LED.guard);
    }
}
