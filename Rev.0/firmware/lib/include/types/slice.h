#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct slice {
    uint32_t capacity;
    uint32_t length;
    bool dynamic;
    uint8_t *bytes;
} slice;

extern slice slice_make(int size);
extern void slice_free(slice *);
extern int slice_append(slice *dest, const slice src);
