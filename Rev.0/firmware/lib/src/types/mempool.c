#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <types/mempool.h>

#define MEMPOOL_CHUNKSIZE 16
#define MEMPOOL_SIZE 32

void mempool_init(mempool *pool) {
    assert(pool != NULL);
    assert(offsetof(memchunk, data) % 4 == 0);

    mutex_init(&pool->guard);

    pool->head = 0;

    memchunk *p = pool->pool;
    for (int i = 0; i < MEMPOOL_SIZE; i++, p++) {
        p->allocated = false;
    }
}

void *mempool_alloc(mempool *pool, size_t N, size_t size) {
    assert(pool != NULL);

    memchunk *chunk = NULL;

    if (N * size > MEMPOOL_CHUNKSIZE) {
        panic("mempool: chunk too large");
    } else if (mutex_try_enter(&pool->guard, NULL)) {
        for (int i = 0; i < MEMPOOL_SIZE; i++) {
            memchunk *p = &pool->pool[pool->head++ % MEMPOOL_SIZE];
            if (!p->allocated) {
                p->allocated = true;
                chunk = p;
                break;
            }
        }

        pool->head = pool->head % MEMPOOL_SIZE;

        mutex_exit(&pool->guard);

        if (chunk == NULL) {
            panic("mempool: out of memory");
        } else {
            memset(chunk->data, 0, MEMPOOL_CHUNKSIZE);
        }
    }

    return chunk ? chunk->data : NULL;
}

void mempool_free(mempool *pool, void *p) {
    assert(pool != NULL);

    if (p != NULL) {
        memchunk *chunk = pool->pool;

        for (int i = 0; i < MEMPOOL_SIZE; i++, chunk++) {
            if (chunk->data == p) {
                chunk->allocated = false;
                break;
            }
        }
    }
}
