#pragma once

#include <stdint.h>

typedef struct PIN {
    uint8_t door;
    char code[16] __attribute__((aligned(4)));
} PIN;

extern void U2_init();
extern void U2_start();
extern void U2_wio(uint8_t inputs);
extern void U2_pin(PIN *pin);
