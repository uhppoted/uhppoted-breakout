#pragma once

typedef struct buffer {
    int N;
    uint8_t data[256];
} buffer;

typedef struct circular_buffer {
    int head;
    int tail;
    uint8_t bytes[256];
} circular_buffer;
