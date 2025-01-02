#pragma once

#include <stddef.h>

extern void mempool_init();
extern void *mempool_calloc(size_t N, size_t size);
extern void mempool_free(void *p);
