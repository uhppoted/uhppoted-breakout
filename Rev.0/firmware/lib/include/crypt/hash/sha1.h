#pragma once

#include <stdint.h>
#include <stdlib.h>

#define SHA1_HASH_LENGTH 20
#define SHA1_BLOCKSIZE 64

typedef struct sha1 {
    union {
        uint8_t b[SHA1_HASH_LENGTH];
        uint32_t w[SHA1_HASH_LENGTH / 4];
    } hash;

    union {
        uint8_t b[SHA1_BLOCKSIZE];
        uint32_t w[SHA1_BLOCKSIZE / 4];
    } buffer;

    uint32_t byteCount;
    uint8_t bufferOffset;

} sha1;

void sha1_init(sha1 *ctx);
void sha1_update(sha1 *ctx, const uint8_t *block, size_t length);
void sha1_finalize(sha1 *ctx);
