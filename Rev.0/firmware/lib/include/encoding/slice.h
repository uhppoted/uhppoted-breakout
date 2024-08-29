#pragma once

#include <stdint.h>

typedef struct slice {
    uint32_t capacity;
    uint32_t length;
    uint8_t *bytes;
} slice;

extern void slice_free(slice *);
extern void slice_append(slice *dest, const slice src);
// uint8_t *slice_reverse(uint8_t *buffer, int N);

