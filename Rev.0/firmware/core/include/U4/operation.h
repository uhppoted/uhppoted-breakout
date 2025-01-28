#pragma once

#include <types/mempool.h>

typedef enum {
    U4_UNKNOWN,
    U4_WRITE,
    U4_HEALTHCHECK,
} U4_TASK;

typedef struct operation {
    U4_TASK tag;
    union {
        struct {
            uint16_t outputs;
        } write;

        struct {
            uint16_t outputs;
        } healthcheck;
    };
} operation;

operation *operation_alloc(mempool *);
void operation_free(mempool *pool, operation *op);
