#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <log.h>
#include <mempool.h>

#define MEMPOOL_CHUNKSIZE 16
#define MEMPOOL_SIZE 32

typedef struct memchunk {
    bool allocated;
    uint8_t data[MEMPOOL_CHUNKSIZE];
} memchunk;

struct {
    mutex_t guard;
    memchunk pool[MEMPOOL_SIZE];
} mempool = {};

void mempool_init() {
    mutex_init(&mempool.guard);

    memchunk *p = mempool.pool;
    for (int i = 0; i < MEMPOOL_SIZE; i++, p++) {
        p->allocated = false;
    }
}

void *mempool_calloc(size_t N, size_t size) {
    memchunk *chunk = NULL;

    if (N * size > MEMPOOL_CHUNKSIZE) {
        panic("mempool: chunk too large");
    } else if (mutex_try_enter(&mempool.guard, NULL)) {
        memchunk *p = mempool.pool;
        for (int i = 0; i < MEMPOOL_SIZE; i++, p++) {
            if (!p->allocated) {
                p->allocated = true;
                chunk = p;
                break;
            }
        }

        mutex_exit(&mempool.guard);

        if (chunk == NULL) {
            panic("out of pool memory");
        } else {
            memset(chunk->data, 0, MEMPOOL_CHUNKSIZE);
        }
    }

    return chunk ? chunk->data : NULL;
}

void mempool_free(void *p) {
    if (p != NULL) {
        memchunk *chunk = mempool.pool;

        for (int i = 0; i < MEMPOOL_SIZE; i++, chunk++) {
            if (chunk->data == p) {
                chunk->allocated = false;
                break;
            }
        }
    }
}
