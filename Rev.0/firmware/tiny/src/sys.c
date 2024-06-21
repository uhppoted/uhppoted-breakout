#include <stdlib.h>
#include <string.h>

#include <hardware/pio.h>
#include <pico/stdlib.h>

#include <U2.h>
#include <U3.h>
#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <state.h>
#include <sys.h>
#include <txrx.h>
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

    // ... CLI
    usb_init();

    return true;
}

void dispatch(uint32_t v) {
    if ((v & MSG) == MSG_DEBUG) {
        debugf("SYS", "... debug??");
    }

    if ((v & MSG) == MSG_WIO) {
        U2_wio(v & 0x0000ffff);
    }

    if ((v & MSG) == MSG_U3) {
        U3_process(v & 0x000000ff);
    }

    if ((v & MSG) == MSG_RX) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
        TXRX_rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_TTY) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
        rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_USB) {
        bool connected = (v & 0x0fffffff) == 1;
        infof("SYS", "USB connected: %s", connected ? "yes" : "no");
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
    uint32_t msg = MSG_TICK;
    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SYS", "blink: queue full");
    }

    return true;
}

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

    pio_sm_put_blocking(pio0, 0, rgb << 8u);
}
