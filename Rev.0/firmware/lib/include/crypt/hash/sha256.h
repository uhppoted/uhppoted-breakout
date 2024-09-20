#pragma once

#include <stdint.h>

#define SHA256_HASH_LENGTH 32
#define SHA256_BLOCKSIZE 64

typedef struct sha256 {
    union {
        uint8_t b[SHA256_HASH_LENGTH];
        uint32_t w[SHA256_HASH_LENGTH / 4];
    } hash;

    union {
        uint8_t b[SHA256_BLOCKSIZE];
        uint32_t w[SHA256_BLOCKSIZE / 4];
    } buffer;

    uint32_t byteCount;
    uint8_t bufferOffset;

} sha256;

void sha256_init(sha256 *ctx);
void sha256_update(sha256 *ctx, const uint8_t *block, size_t length);
void sha256_finalize(sha256 *ctx);
