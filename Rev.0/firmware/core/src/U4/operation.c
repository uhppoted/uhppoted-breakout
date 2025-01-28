#include <U4/operation.h>

operation *operation_alloc(mempool *pool) {
    if (pool != NULL) {
        return (operation *)mempool_alloc(pool, 1, sizeof(operation));
    }

    return NULL;
}

void operation_free(mempool *pool, operation *op) {
    if (pool != NULL && op != NULL) {
        mempool_free(pool, op);
    }
}
