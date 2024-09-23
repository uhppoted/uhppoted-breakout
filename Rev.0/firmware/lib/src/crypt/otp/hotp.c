#include <string.h>

#include <crypt/hmac/hmac_sha1.h>
#include <crypt/otp/hotp.h>

uint32_t hotp_generate(const uint8_t *secret, int length, uint64_t counter);

bool hotp_validate(const uint8_t *secret, int length, uint64_t counter, uint32_t code) {
    uint32_t otp = hotp_generate(secret, length, counter);

    return code == otp;
}

uint32_t hotp_generate(const uint8_t *secret, int length, uint64_t counter) {
    // ... calculate HMAC
    uint64_t v = counter;
    uint8_t buffer[8];
    for (int i = 0; i < 8; i++) {
        buffer[sizeof(buffer) - i - 1] = (uint8_t)(v & 0x00ff);
        v >>= 8;
    }

    hmac_sha1 ctx;
    uint8_t hmac[20];

    hmac_sha1_init(&ctx, secret, length);
    hmac_sha1_update(&ctx, buffer, 8);
    hmac_sha1_finalize(&ctx, hmac);

    // ... truncate to 6 digits (RFC 4226)
    uint16_t offset = hmac[sizeof(hmac) - 1] & 0x0f;
    uint32_t value = ((uint32_t)(hmac[offset] & 0x7f) << 24) |
                     ((uint32_t)(hmac[offset + 1] & 0xff) << 16) |
                     ((uint32_t)(hmac[offset + 2] & 0xff) << 8) |
                     ((uint32_t)(hmac[offset + 3] & 0xff) << 0);

    return value % 1000000;
}
