#pragma once

#include <stddef.h>

#include <pico/sync.h>

#include <types/datetime.h>
#include <types/operation.h>
#include <types/swipe.h>

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

bool mempool_init();

struct operation *operation_alloc();
struct swipe *swipe_alloc();
struct datetime *datetime_alloc();

void operation_free(struct operation *op);
void swipe_free(struct swipe *swipe);
void datetime_free(datetime *dt);
