#pragma once

#include <stdint.h>

#define HASH_LENGTH 32
#define BLOCK_LENGTH 64

typedef struct sha256 {
    union {
        uint8_t b[HASH_LENGTH];
        uint32_t w[HASH_LENGTH / 4];
    } hash;

    union {
        uint8_t b[BLOCK_LENGTH];
        uint32_t w[BLOCK_LENGTH / 4];
    } buffer;

    uint32_t byteCount;
    uint8_t bufferOffset;

} sha256;

void sha256_init(sha256 *ctx);
void sha256_update(sha256 *ctx, uint8_t *block, size_t length);
void sha256_finalize(sha256 *ctx);
