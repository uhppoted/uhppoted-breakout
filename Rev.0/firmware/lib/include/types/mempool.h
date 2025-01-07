#pragma once

#include <stddef.h>

#include <pico/sync.h>

typedef struct memchunk {
    bool allocated;
    uint8_t data[16] __attribute__((aligned(4)));
} memchunk;

typedef struct mempool {
    mutex_t guard;
    uint32_t head;
    struct memchunk pool[32] __attribute__((aligned(4)));
} mempool;

extern void mempool_init(mempool *pool);
extern void *mempool_alloc(mempool *pool, size_t N, size_t size);
extern void mempool_free(mempool *pool, void *p);
