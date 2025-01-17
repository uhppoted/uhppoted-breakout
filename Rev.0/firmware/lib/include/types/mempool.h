#pragma once

#include <stddef.h>

#include <pico/sync.h>

struct mempool;

typedef struct memchunk {
    bool allocated;
    struct mempool *pool __attribute__((aligned(4)));
    uint8_t data[16] __attribute__((aligned(4)));
} memchunk;

typedef struct mempool {
    uint32_t size;
    uint32_t chunksize;
    uint32_t head;
    mutex_t guard;
    struct memchunk **pool __attribute__((aligned(4)));
} mempool;

extern bool mempool_init(mempool *pool, uint32_t size, uint32_t chunksize);
extern void *mempool_alloc(mempool *pool, size_t N, size_t size);
extern void mempool_free(mempool *pool, void *p);
