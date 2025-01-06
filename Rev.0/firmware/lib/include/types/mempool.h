#pragma once

#include <stddef.h>

#include <pico/sync.h>

typedef struct memchunk {
    bool allocated;
    uint8_t data[16];
} memchunk;

typedef struct mempool {
    mutex_t guard;
    struct memchunk pool[32];
} mempool;

extern void mempool_init(mempool *pool);
extern void *mempool_alloc(mempool *pool, size_t N, size_t size);
extern void mempool_free(mempool *pool, void *p);
