#include <stdint.h>
#include <string.h>

#include <log.h>
#include <mempool.h>

#define MEMPOOL_CHUNKSIZE 16
#define MEMPOOL_SIZE 32

struct {
    volatile int head;
    volatile int tail;
    uint8_t pool[MEMPOOL_SIZE][MEMPOOL_CHUNKSIZE];
} mempool = {
    .head = MEMPOOL_SIZE - 1,
    .tail = 0,
};

void mempool_init() {
}

void *mempool_calloc(size_t N, size_t size) {
    int tail = mempool.tail;
    void *chunk = NULL;

    if (tail != mempool.head) {
        chunk = mempool.pool[tail++];
        mempool.tail = tail % MEMPOOL_SIZE;

        memset(chunk, 0, MEMPOOL_CHUNKSIZE);
    }

    return chunk;
}

void mempool_free(void *p) {
    if (p != NULL) {
        int head = mempool.head;
        int next = (head + 1) % MEMPOOL_SIZE;

        if (next != mempool.tail) {
            mempool.head = next;
        }
    }
}
