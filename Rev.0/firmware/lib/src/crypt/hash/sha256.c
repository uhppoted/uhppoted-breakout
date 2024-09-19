// Ref. https://github.com/Cathedrow/Cryptosuite

#include <string.h>

#include <crypt/hash/sha256.h>

#define BUFFER_SIZE 64

const uint32_t SHA256K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const uint8_t SHA256_INITIAL_STATE[] = {
    0x67, 0xe6, 0x09, 0x6a, // H0
    0x85, 0xae, 0x67, 0xbb, // H1
    0x72, 0xf3, 0x6e, 0x3c, // H2
    0x3a, 0xf5, 0x4f, 0xa5, // H3
    0x7f, 0x52, 0x0e, 0x51, // H4
    0x8c, 0x68, 0x05, 0x9b, // H5
    0xab, 0xd9, 0x83, 0x1f, // H6
    0x19, 0xcd, 0xe0, 0x5b  // H7
};

void sha256_hashblock(sha256 *ctx);
void sha256_pad(sha256 *ctx);
void sha256_padWithByte(sha256 *ctx, uint8_t data);

inline uint32_t sha256_ror32(uint32_t number, uint8_t bits) {
    return ((number << (32 - bits)) | (number >> bits));
}

void sha256_init(sha256 *ctx) {
    memmove(ctx->hash.b, SHA256_INITIAL_STATE, sizeof(ctx->hash.b));

    ctx->byteCount = 0;
    ctx->bufferOffset = 0;
}

void sha256_update(sha256 *ctx, const uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        ctx->byteCount++;

        ctx->buffer.b[ctx->bufferOffset ^ 3] = data[i];
        ctx->bufferOffset++;
        if (ctx->bufferOffset == BUFFER_SIZE) {
            sha256_hashblock(ctx);
            ctx->bufferOffset = 0;
        }
    }
}

void sha256_finalize(sha256 *ctx) {
    sha256_pad(ctx);

    // ... swap byte order back
    for (int i = 0; i < 8; i++) {
        uint32_t a, b;

        a = ctx->hash.w[i];
        b = a << 24;
        b |= (a << 8) & 0x00ff0000;
        b |= (a >> 8) & 0x0000ff00;
        b |= a >> 24;

        ctx->hash.w[i] = b;
    }
}

void sha256_hashblock(sha256 *ctx) {
    uint8_t i;
    uint32_t a, b, c, d, e, f, g, h, t1, t2;

    a = ctx->hash.w[0];
    b = ctx->hash.w[1];
    c = ctx->hash.w[2];
    d = ctx->hash.w[3];
    e = ctx->hash.w[4];
    f = ctx->hash.w[5];
    g = ctx->hash.w[6];
    h = ctx->hash.w[7];

    for (i = 0; i < 64; i++) {
        if (i >= 16) {
            t1 = ctx->buffer.w[i & 15] + ctx->buffer.w[(i - 7) & 15];
            t2 = ctx->buffer.w[(i - 2) & 15];
            t1 += sha256_ror32(t2, 17) ^ sha256_ror32(t2, 19) ^ (t2 >> 10);
            t2 = ctx->buffer.w[(i - 15) & 15];
            t1 += sha256_ror32(t2, 7) ^ sha256_ror32(t2, 18) ^ (t2 >> 3);

            ctx->buffer.w[i & 15] = t1;
        }

        t1 = h;
        t1 += sha256_ror32(e, 6) ^ sha256_ror32(e, 11) ^ sha256_ror32(e, 25); // ∑1(e)
        t1 += g ^ (e & (g ^ f));                                              // Ch(e,f,g)
        t1 += SHA256K[i];                                                     // Ki
        t1 += ctx->buffer.w[i & 15];                                          // Wi
        t2 = sha256_ror32(a, 2) ^ sha256_ror32(a, 13) ^ sha256_ror32(a, 22);  // ∑0(a)
        t2 += ((b & c) | (a & (b | c)));                                      // Maj(a,b,c)

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->hash.w[0] += a;
    ctx->hash.w[1] += b;
    ctx->hash.w[2] += c;
    ctx->hash.w[3] += d;
    ctx->hash.w[4] += e;
    ctx->hash.w[5] += f;
    ctx->hash.w[6] += g;
    ctx->hash.w[7] += h;
}

// FIPS180-2 §5.1.1 compliant SHA-256 padding.
void sha256_pad(sha256 *ctx) {
    // ... pad with 0x80 followed by 0x00 until the end of the block
    sha256_padWithByte(ctx, 0x80);
    while (ctx->bufferOffset != 56) {
        sha256_padWithByte(ctx, 0x00);
    }

    // ... append length in the last 8 bytes
    sha256_padWithByte(ctx, 0);                    // We're only using 32 bit lengths
    sha256_padWithByte(ctx, 0);                    // But SHA-1 supports 64 bit lengths
    sha256_padWithByte(ctx, 0);                    // So zero pad the top bits
    sha256_padWithByte(ctx, ctx->byteCount >> 29); // Shifting to multiply by 8
    sha256_padWithByte(ctx, ctx->byteCount >> 21); // as SHA-1 supports bitstreams as well as
    sha256_padWithByte(ctx, ctx->byteCount >> 13); // byte.
    sha256_padWithByte(ctx, ctx->byteCount >> 5);
    sha256_padWithByte(ctx, ctx->byteCount << 3);
}

void sha256_padWithByte(sha256 *ctx, uint8_t data) {
    ctx->buffer.b[ctx->bufferOffset ^ 3] = data;
    ctx->bufferOffset++;

    if (ctx->bufferOffset == BUFFER_SIZE) {
        sha256_hashblock(ctx);
        ctx->bufferOffset = 0;
    }
}
