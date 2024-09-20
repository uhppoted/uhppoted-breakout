#include <string.h>

#include <crypt/hmac/hmac_sha1.h>

const uint8_t HMAC_IPAD = 0x36;
const uint8_t HMAC_OPAD = 0x5c;

void hmac_sha1_init(hmac_sha1 *ctx, const uint8_t key[], size_t length) {
    memset(ctx->key, 0, SHA1_BLOCKSIZE);

    if (length > SHA1_BLOCKSIZE) {
        sha1_init(&ctx->sha1);
        sha1_update(&ctx->sha1, key, length);
        sha1_finalize(&ctx->sha1);

        memmove(ctx->key, ctx->sha1.hash.b, SHA1_HASH_LENGTH);
    } else {
        memmove(ctx->key, key, length);
    }

    // ... inner hash
    uint8_t iv[SHA1_BLOCKSIZE];

    memset(iv, HMAC_IPAD, SHA1_BLOCKSIZE);
    for (int i = 0; i < SHA1_BLOCKSIZE; i++) {
        iv[i] ^= ctx->key[i];
    }

    sha1_init(&ctx->sha1);
    sha1_update(&ctx->sha1, iv, SHA1_BLOCKSIZE);
}

void hmac_sha1_update(hmac_sha1 *ctx, const uint8_t *block, size_t length) {
    sha1_update(&ctx->sha1, block, length);
}

void hmac_sha1_finalize(hmac_sha1 *ctx) {
    // ... complete inner hash

    uint8_t inner[SHA1_BLOCKSIZE];

    sha1_finalize(&ctx->sha1);
    memmove(inner, ctx->sha1.hash.b, SHA1_HASH_LENGTH);

    // ... calculate outer hash
    uint8_t fv[SHA1_BLOCKSIZE];

    memset(fv, HMAC_OPAD, SHA1_BLOCKSIZE);
    for (int i = 0; i < SHA1_BLOCKSIZE; i++) {
        fv[i] ^= ctx->key[i];
    }

    sha1_init(&ctx->sha1);
    sha1_update(&ctx->sha1, fv, SHA1_BLOCKSIZE);
    sha1_update(&ctx->sha1, inner, SHA1_BLOCKSIZE);
    sha1_finalize(&ctx->sha1);
}
