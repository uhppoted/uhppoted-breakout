#include <stdio.h>

#include <hardware/pio.h>
#include <pico/stdlib.h>

#include "ws2812.pio.h"

const char *TERMINAL_CLEAR = "\033c\033[2J";

void put_rgb(uint8_t red, uint8_t green, uint8_t blue);

struct {
    bool LED;
} sys = {
    .LED = false,
};

bool sys_init() {
    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);
    put_rgb(128, 0, 0);

    // ... print startup message
    printf(TERMINAL_CLEAR);
    printf("-----  BREAKOUT   TEST\n");

    return true;
}

/* Blinks SYSLED.
 *
 */
void sys_blink() {
    sys.LED = !sys.LED;

    if (sys.LED) {
        put_rgb(8, 4, 0); // yellow'ish
    } else {
        put_rgb(8, 0, 64); // purple'ish
    }
}

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t rgb = (red << 16u) | (green << 8u) | (blue / 16 << 0u);

    pio_sm_put_blocking(pio0, 0, rgb << 8u);
}
