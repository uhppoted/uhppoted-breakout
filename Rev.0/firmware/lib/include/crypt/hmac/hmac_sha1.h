#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <crypt/hash/sha1.h>

typedef struct hmac_sha1 {
    sha1 sha1;
    uint8_t key[SHA1_BLOCKSIZE];
} hmac_sha1;

void hmac_sha1_init(hmac_sha1 *ctx, const uint8_t key[], size_t length);
void hmac_sha1_update(hmac_sha1 *ctx, const uint8_t *block, size_t length);
void hmac_sha1_finalize(hmac_sha1 *ctx, uint8_t hmac[20]);
