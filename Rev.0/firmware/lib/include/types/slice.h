#pragma once

#include <stdint.h>

typedef struct slice {
    uint32_t capacity;
    uint32_t length;
    uint8_t *bytes;
} slice;

extern slice make_slice(int size);
extern void slice_free(slice *const);
extern void slice_append(slice *constdest, const slice src);
// uint8_t *slice_reverse(uint8_t *buffer, int N);
