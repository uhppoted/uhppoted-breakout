#pragma once

typedef struct swipe {
    uint8_t door;
    char card[16] __attribute__((aligned(4)));
    char code[16] __attribute__((aligned(4)));
} swipe;
