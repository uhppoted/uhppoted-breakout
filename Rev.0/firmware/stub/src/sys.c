#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <hardware/pio.h>
#include <hardware/ticks.h>

#include <log.h>
#include <stub.h>
#include <sys.h>

#include "ws2812.pio.h"

bool _tick(repeating_timer_t *t);

void put_rgb(uint8_t red, uint8_t green, uint8_t blue);

struct {
    repeating_timer_t timer;
    uint32_t ticks;

    struct {
        bool LED;
        mutex_t guard;
    } LED;

} sys = {
    .ticks = 0,
    .LED =
        {
            .LED = false,
        },
};

// struct {
//     repeating_timer_t timer;
//     uint32_t ticks;
//     mode mode;
//     bool reboot;

//     struct {
//         mutex_t lock;
//         int head;
//         int tail;
//         char list[PRINT_QUEUE_SIZE][128];
//     } queue;

//     struct {
//         absolute_time_t touched;
//     } cli;

//     struct {
//         float interval;
//         absolute_time_t touched;
//         repeating_timer_t timer;
//     } trace;

//     mutex_t guard;
// } SYSTEM = {
//     .ticks = 0,
//     .mode = MODE_NONE,
//     .reboot = false,
//     .queue = {
//         .head = 0,
//         .tail = 0,
//     },
//     .cli = {
//         .touched = 0,
//     },
//     .trace = {
//         .interval = (float)TRACE,
//         .touched = 0,
//     }};

bool sys_init() {
    mutex_init(&sys.LED.guard);

    // ... WS2812 LED
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, 16, 800000, true);
    put_rgb(128, 12, 0);

    // // ... initialise
    // mutex_init(&SYSTEM.guard);
    // mutex_init(&SYSTEM.queue.lock);

    // ... system tick
    if (!add_repeating_timer_ms(1000, _tick, &sys, &sys.timer)) {
        return false;
    }

    // ... startup message
    char s[64];

    snprintf(s, sizeof(s), "-----  STUB   v%02x.%02x\n",
             (VERSION >> 8) & 0x00ff, (VERSION >> 0) & 0x00ff);

    printf("%s\n", s);

    return true;
}

bool _tick(repeating_timer_t *t) {
    sys.ticks++;

    message qmsg = {
        .message = MSG_TICK,
        .tag = MESSAGE_NONE,
    };

    push(qmsg);

    return true;
}

/* Blinks SYSLED.
 *
 */
void sys_tick() {
    uint16_t errors = 0; // get_errors();

    sys.LED.LED = !sys.LED.LED;

    if (sys.LED.LED && errors != 0x0000) {
        put_rgb(96, 0, 0);
    } else if (sys.LED.LED) {
        put_rgb(0, 8, 0);
    } else {
        put_rgb(32, 0, 96); // purple
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
