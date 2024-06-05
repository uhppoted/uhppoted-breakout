#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/pio.h>
#include <pico/stdlib.h>

#include "ws2812.pio.h"
#include <usb.h>

// #include <breakout.h>
// #include <dio.h>
// #include <readers.h>
// #include <sys.h>

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

// void dispatch(uint32_t v) {
//     if ((v & MSG) == MSG_RX) {
//         char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
//         rx(b);
//         free(b);
//     }
//
//     if ((v & MSG) == MSG_WIO) {
//         wio((uint8_t)(v & 0x0fffffff));
//     }
//
//     if ((v & MSG) == MSG_INPUTS) {
//         inputs((uint8_t)(v & 0x0fffffff));
//     }
// }

// void on_uart0_rx() {
//     char buffer[32];
//     int ix = 0;
//
//     while (uart_is_readable(UART0) && ix < sizeof(buffer)) {
//         buffer[ix++] = uart_getc(UART0);
//     }
//
//     if (ix > 0) {
//         char *b;
//
//         if ((b = calloc(ix + 1, 1)) != NULL) {
//             memmove(b, buffer, ix);
//             uint32_t msg = MSG_RX | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
//             if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
//                 free(b);
//             }
//         }
//
//         ix = 0;
//     }
// }

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
