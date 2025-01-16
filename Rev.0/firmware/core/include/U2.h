#pragma once

#include <stdint.h>

typedef struct swipe {
    uint8_t door;
    char card[16] __attribute__((aligned(4)));
    char code[16] __attribute__((aligned(4)));
} swipe;

extern void U2_init();
extern void U2_start();
extern void U2_wio(uint8_t inputs);
extern void U2_free(swipe *swipe);
