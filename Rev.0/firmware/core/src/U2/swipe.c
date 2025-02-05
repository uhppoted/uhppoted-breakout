#include <U2/swipe.h>

swipe *swipe_alloc(mempool *pool) {
    if (pool != NULL) {
        return (swipe *)mempool_alloc(pool, 1, sizeof(swipe));
    }

    return NULL;
}

void swipe_free(mempool *pool, swipe *swipe) {
    if (pool != NULL && swipe != NULL) {
        mempool_free(pool, swipe);
    }
}
