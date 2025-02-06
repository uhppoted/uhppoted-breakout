#include <stdlib.h>
#include <string.h>

#include <mempool.h>

#define MEMPOOL_CHUNKSIZE 128
#define MEMPOOL_SIZE 64

bool pool_init(mempool *pool, uint32_t size, uint32_t chunksize);
void *pool_alloc(mempool *pool, size_t N, size_t size);
void pool_free(mempool *pool, void *p);

struct {
    mempool operation;
    mempool swipe;
    mempool datetime;
} pools;

bool mempool_init() {
    if (!pool_init(&pools.operation, 32, sizeof(operation))) {
        return false;
    }

    if (!pool_init(&pools.swipe, 16, sizeof(swipe))) {
        return false;
    }

    if (!pool_init(&pools.datetime, 8, sizeof(swipe))) {
        return false;
    }

    return true;
}

operation *operation_alloc() {
    return (operation *)pool_alloc(&pools.operation, 1, sizeof(operation));
}

void operation_free(operation *op) {
    pool_free(&pools.operation, op);
}

swipe *swipe_alloc(mempool *pool) {
    return (swipe *)pool_alloc(&pools.swipe, 1, sizeof(swipe));
}

void swipe_free(swipe *swipe) {
    pool_free(&pools.swipe, swipe);
}

datetime *datetime_alloc(mempool *pool) {
    return (datetime *)pool_alloc(&pools.datetime, 1, sizeof(datetime));
}

void datetime_free(datetime *dt) {
    pool_free(&pools.datetime, dt);
}

bool pool_init(mempool *pool, uint32_t size, uint32_t chunksize) {
    assert(pool != NULL);
    assert(offsetof(memchunk, data) % 4 == 0);
    assert(size <= MEMPOOL_SIZE);
    assert(chunksize <= MEMPOOL_CHUNKSIZE);

    mutex_init(&pool->guard);

    pool->head = 0;
    pool->size = 0;
    pool->chunksize = chunksize;

    // ... round up pool size to power of 2
    // Ref. https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
    uint32_t chunks = size < 2 ? 2 : size;

    chunks--;
    chunks |= chunks >> 1;
    chunks |= chunks >> 2;
    chunks |= chunks >> 4;
    chunks |= chunks >> 8;
    chunks |= chunks >> 16;
    chunks++;

    // ... allocate chunks list
    pool->pool = (memchunk **)calloc(chunks, sizeof(memchunk *));
    if (pool->pool == NULL) {
        return false;
    }

    for (int i = 0; i < chunks; i++) {
        pool->pool[i] = NULL;
    }

    // ... pad chunksize 'for in case' and round up to multiple of 4 bytes
    uint32_t bytes = chunksize + 1;
    while ((bytes % 4) != 0) {
        bytes++;
    }

    // ... allocate pool chunks
    for (int i = 0; i < size; i++) {
        memchunk *p = (memchunk *)calloc(1, bytes);
        if (p != NULL) {
            p->allocated = false;
            p->pool = pool;
            pool->pool[pool->size++] = p;
        }
    }

    // ... good to go
    return true;
}

void *pool_alloc(mempool *pool, size_t N, size_t size) {
    assert(pool != NULL);

    const uint32_t poolsize = pool->size;
    const uint32_t chunksize = pool->chunksize;
    memchunk *chunk = NULL;

    if (N * size > chunksize) {
        panic("mempool::alloc chunk too large");
    } else if (mutex_try_enter(&pool->guard, NULL)) {
        for (int i = 0; i < poolsize; i++) {
            memchunk *p = pool->pool[pool->head++ % poolsize];
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

void pool_free(mempool *pool, void *p) {
    assert(pool != NULL);

    const uint32_t poolsize = pool->size;

    if (p != NULL) {
        for (int i = 0; i < poolsize; i++) {
            memchunk *chunk = pool->pool[i];

            if (chunk->data == p) {
                chunk->allocated = false;
                break;
            }
        }
    }
}
