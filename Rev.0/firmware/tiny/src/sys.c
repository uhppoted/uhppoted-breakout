#include <hardware/pio.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <log.h>
#include <state.h>
#include <usb.h>

#include "ws2812.pio.h"

bool on_tick(repeating_timer_t *);
void put_rgb(uint8_t red, uint8_t green, uint8_t blue);

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

    if (!add_repeating_timer_ms(1000, on_tick, &sysled, &sysled.timer)) {
        return false;
    }

    // ... system stuff
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

/* Blinks SYSLED.
 *
 */
void tick() {
    sysled.state = !sysled.state;

    if (sysled.state) {
        put_rgb(8, 0, 0);
    } else {
        put_rgb(0, 8, 0);
    }
}

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

    pio_sm_put_blocking(pio0, 0, rgb << 8u);
}
