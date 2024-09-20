// Ref. https://github.com/Cathedrow/Cryptosuite

#include <string.h>

#include <crypt/hash/sha1.h>

#define SHA1_K0 0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

uint8_t SHA1_INITIAL_STATE[] = {
    0x01, 0x23, 0x45, 0x67, // H0
    0x89, 0xab, 0xcd, 0xef, // H1
    0xfe, 0xdc, 0xba, 0x98, // H2
    0x76, 0x54, 0x32, 0x10, // H3
    0xf0, 0xe1, 0xd2, 0xc3  // H4
};

void sha1_hashblock(sha1 *ctx);
void sha1_pad(sha1 *ctx);
void sha1_padWithByte(sha1 *ctx, uint8_t data);

inline uint32_t rol32(uint32_t number, uint8_t bits) {
    return ((number << bits) | (number >> (32 - bits)));
}

void sha1_init(sha1 *ctx) {
    memmove(ctx->hash.b, SHA1_INITIAL_STATE, SHA1_HASH_LENGTH);

    ctx->byteCount = 0;
    ctx->bufferOffset = 0;
}

void sha1_update(sha1 *ctx, const uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        ctx->byteCount++;

        ctx->buffer.b[ctx->bufferOffset ^ 3] = data[i];
        ctx->bufferOffset++;
        if (ctx->bufferOffset == SHA1_BLOCKSIZE) {
            sha1_hashblock(ctx);
            ctx->bufferOffset = 0;
        }
    }
}

void sha1_finalize(sha1 *ctx) {
    sha1_pad(ctx);

    // ... swap byte order back
    for (int i = 0; i < 5; i++) {
        uint32_t a, b;

        a = ctx->hash.w[i];
        b = a << 24;
        b |= (a << 8) & 0x00ff0000;
        b |= (a >> 8) & 0x0000ff00;
        b |= a >> 24;

        ctx->hash.w[i] = b;
    }
}

void sha1_hashblock(sha1 *ctx) {
    uint8_t i;
    uint32_t a, b, c, d, e, t;

    a = ctx->hash.w[0];
    b = ctx->hash.w[1];
    c = ctx->hash.w[2];
    d = ctx->hash.w[3];
    e = ctx->hash.w[4];

    for (i = 0; i < 80; i++) {
        if (i >= 16) {
            t = ctx->buffer.w[(i + 13) & 15] ^ ctx->buffer.w[(i + 8) & 15] ^ ctx->buffer.w[(i + 2) & 15] ^ ctx->buffer.w[i & 15];
            ctx->buffer.w[i & 15] = rol32(t, 1);
        }

        if (i < 20) {
            t = (d ^ (b & (c ^ d))) + SHA1_K0;
        } else if (i < 40) {
            t = (b ^ c ^ d) + SHA1_K20;
        } else if (i < 60) {
            t = ((b & c) | (d & (b | c))) + SHA1_K40;
        } else {
            t = (b ^ c ^ d) + SHA1_K60;
        }

        t += rol32(a, 5) + e + ctx->buffer.w[i & 15];
        e = d;
        d = c;
        c = rol32(b, 30);
        b = a;
        a = t;
    }

    ctx->hash.w[0] += a;
    ctx->hash.w[1] += b;
    ctx->hash.w[2] += c;
    ctx->hash.w[3] += d;
    ctx->hash.w[4] += e;
}

// FIPS180-2 §5.1.1 compliant SHA-1 padding.
void sha1_pad(sha1 *ctx) {
    // ... pad with 0x80 followed by 0x00 until the end of the block
    sha1_padWithByte(ctx, 0x80);
    while (ctx->bufferOffset != 56) {
        sha1_padWithByte(ctx, 0x00);
    }

    // ... append length in the last 8 bytes
    sha1_padWithByte(ctx, 0);                    // We're only using 32 bit lengths
    sha1_padWithByte(ctx, 0);                    // But SHA-1 supports 64 bit lengths
    sha1_padWithByte(ctx, 0);                    // So zero pad the top bits
    sha1_padWithByte(ctx, ctx->byteCount >> 29); // Shifting to multiply by 8
    sha1_padWithByte(ctx, ctx->byteCount >> 21); // as SHA-1 supports bitstreams as well as
    sha1_padWithByte(ctx, ctx->byteCount >> 13); // byte.
    sha1_padWithByte(ctx, ctx->byteCount >> 5);
    sha1_padWithByte(ctx, ctx->byteCount << 3);
}

void sha1_padWithByte(sha1 *ctx, uint8_t data) {
    ctx->buffer.b[ctx->bufferOffset ^ 3] = data;
    ctx->bufferOffset++;

    if (ctx->bufferOffset == SHA1_BLOCKSIZE) {
        sha1_hashblock(ctx);
        ctx->bufferOffset = 0;
    }
}
