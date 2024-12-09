#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct circular_buffer {
    volatile int head;
    volatile int tail;
    uint8_t bytes[256];
} circular_buffer;

bool buffer_push(circular_buffer *buffer, uint8_t ch);
bool buffer_pop(circular_buffer *buffer, uint8_t *ch);
bool buffer_empty(circular_buffer *buffer);
void buffer_flush(circular_buffer *buffer, void (*f)(uint8_t));
