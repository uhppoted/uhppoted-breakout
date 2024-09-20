/*
 * Validation test suite for HMAC SHA-1 implementation.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <crypt/hmac/hmac_sha1.h>

bool test_HMAC_SHA1() {
    bool ok = false;

    printf("-- HMAC SHA-1\n");

    // ... calculate hash
    uint8_t key[]={
   0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
   0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
   0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f
};

    char *expected = "4f4ca3d5d68ba7cc0a1208c9c61e9c5da0403c0a";
    hmac_sha1 ctx;

    hmac_sha1_init(&ctx, key, sizeof(key));
    hmac_sha1_update(&ctx,"Sample #1",9);
    hmac_sha1_finalize(&ctx);

    // ... convert to hex
    char hmac[41] = "???";
    int ix = 0;

    for (int i = 0; i < 20; i++) {
        ix += snprintf(&hmac[ix], sizeof(hmac) - ix, "%02x", ctx.sha1.hash.b[i]);
    }

    // ... print result
    if (strcmp(hmac, expected) == 0) {
        ok = true;
    }

    printf("%-5s  %s\n", ok ? "ok" : "error", "FIPS 198a A.1");
    if (!ok) {
        printf("       - expected: %s\n", expected);
        printf("       - got:      %s\n", hmac);
    }

    return ok;
}
