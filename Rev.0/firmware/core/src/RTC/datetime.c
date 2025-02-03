#include <RTC/datetime.h>

datetime *datetime_alloc(mempool *pool) {
    if (pool != NULL) {
        return (datetime *)mempool_alloc(pool, 1, sizeof(datetime));
    }

    return NULL;
}

void datetime_free(mempool *pool, datetime *dt) {
    if (pool != NULL && dt != NULL) {
        mempool_free(pool, dt);
    }
}
