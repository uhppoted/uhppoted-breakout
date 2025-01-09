#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <types/mempool.h>

#define MEMPOOL_CHUNKSIZE 16
#define MEMPOOL_SIZE 32

void mempool_init(mempool *pool, uint32_t size, uint32_t chunksize) {
    assert(pool != NULL);
    assert(offsetof(memchunk, data) % 4 == 0);
    assert(size <= MEMPOOL_SIZE);
    assert(chunksize <= MEMPOOL_CHUNKSIZE);

    mutex_init(&pool->guard);

    pool->head = 0;
    pool->size = 0;
    pool->chunksize = chunksize;

    memchunk *p = pool->pool;
    for (int i = 0; i < size; i++, p++) {
        p->allocated = false;
        pool->size++;
    }
}

void *mempool_alloc(mempool *pool, size_t N, size_t size) {
    assert(pool != NULL);

    const uint32_t poolsize = pool->size;
    const uint32_t chunksize = pool->chunksize;
    memchunk *chunk = NULL;

    if (N * size > chunksize) {
        panic("mempool::alloc chunk too large");
    } else if (mutex_try_enter(&pool->guard, NULL)) {
        for (int i = 0; i < poolsize; i++) {
            memchunk *p = &pool->pool[pool->head++ % MEMPOOL_SIZE];
            if (!p->allocated) {
                p->allocated = true;
                chunk = p;
                break;
            }
        }

        pool->head = pool->head % poolsize;

        mutex_exit(&pool->guard);

        if (chunk == NULL) {
            panic("mempool: out of memory");
        } else {
            memset(chunk->data, 0, chunksize);
        }
    }

    return chunk ? chunk->data : NULL;
}

void mempool_free(mempool *pool, void *p) {
    assert(pool != NULL);

    const uint32_t poolsize = pool->size;

    if (p != NULL) {
        memchunk *chunk = pool->pool;

        for (int i = 0; i < poolsize; i++, chunk++) {
            if (chunk->data == p) {
                chunk->allocated = false;
                break;
            }
        }
    }
}
