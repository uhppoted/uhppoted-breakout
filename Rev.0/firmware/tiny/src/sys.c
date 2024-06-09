#include <stdlib.h>
#include <string.h>

#include <hardware/pio.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <cli.h>
#include <dio.h>
#include <log.h>
#include <readers.h>
#include <sys.h>
#include <usb.h>

#include "ws2812.pio.h"

void put_rgb(uint8_t red, uint8_t green, uint8_t blue);
bool blink(repeating_timer_t *);

struct {
    struct repeating_timer timer;
    bool state;
} sysled = {
    .state = false,
};

bool sysinit() {
    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);

    if (!add_repeating_timer_ms(1000, blink, &sysled, &sysled.timer)) {
        return false;
    }

    // ... USB
    setup_usb();

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
        sysled.state = !sysled.state;

        if (sysled.state) {
            put_rgb(8, 0, 0);
        } else {
            put_rgb(0, 8, 0);
        }
    }
}

/* SYSLED blink callback.
 *
 */
bool blink(repeating_timer_t *t) {
    uint32_t msg = MSG_TICK | ((uint32_t)inputs & 0x00000000);
    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        warnf("SYS", "tick: queue full");
    }

    return true;
}

// void blink() {
//     uint8_t cnt = 0;
//
//     for (cnt = 0; cnt < 0xff; cnt++) {
//         put_rgb(cnt, 0xff - cnt, 0);
//         sleep_ms(3);
//     }
//     for (cnt = 0; cnt < 0xff; cnt++) {
//         put_rgb(0xff - cnt, 0, cnt);
//         sleep_ms(3);
//     }
//     for (cnt = 0; cnt < 0xff; cnt++) {
//         put_rgb(0, cnt, 0xff - cnt);
//         sleep_ms(3);
//     }
// }

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

    pio_sm_put_blocking(pio0, 0, rgb << 8u);
}
