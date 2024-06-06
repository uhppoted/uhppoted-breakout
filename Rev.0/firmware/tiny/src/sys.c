#include <stdlib.h>
#include <string.h>

#include <hardware/pio.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <cli.h>
#include <dio.h>
#include <readers.h>
#include <sys.h>
#include <usb.h>

#include "ws2812.pio.h"

void put_rgb(uint8_t red, uint8_t green, uint8_t blue);

void sysinit() {
    // ... USB
    setup_usb();

    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);
}

void dispatch(uint32_t v) {
    if ((v & MSG) == MSG_RX) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
        rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_WIO) {
        wio((uint8_t)(v & 0x0fffffff));
    }

    if ((v & MSG) == MSG_INPUTS) {
        inputs((uint8_t)(v & 0x0fffffff));
    }
}

void blink() {
    uint8_t cnt = 0;

    for (cnt = 0; cnt < 0xff; cnt++) {
        put_rgb(cnt, 0xff - cnt, 0);
        sleep_ms(3);
    }
    for (cnt = 0; cnt < 0xff; cnt++) {
        put_rgb(0xff - cnt, 0, cnt);
        sleep_ms(3);
    }
    for (cnt = 0; cnt < 0xff; cnt++) {
        put_rgb(0, cnt, 0xff - cnt);
        sleep_ms(3);
    }
}

void put_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t grb = (green / 16 << 16) | (red / 16 << 8) | (blue / 16 << 0);

    pio_sm_put_blocking(pio0, 0, grb << 8u);
}
