/*
 * Validation test suite for SHA-256 implementation.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <crypt/hash/sha256.h>

bool test_SHA256() {
    bool ok = false;

    printf("-- SHA-256\n");

    // ... calculate hash
    char *expected = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
    sha256 ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, "abc", 3);
    sha256_finalize(&ctx);

    // ... convert to hex
    char hash[65] = "???";
    int ix = 0;

    for (int i = 0; i < 32; i++) {
        ix += snprintf(&hash[ix], sizeof(hash) - ix, "%02x", ctx.hash.b[i]);
    }

    // ... print result
    if (strcmp(hash, expected) == 0) {
        ok = true;
    }

    printf("%-5s  %s\n", ok ? "ok" : "error", "FIPS 180-2 B.1");
    if (!ok) {
        printf("       - expected: %s\n", expected);
        printf("       - got:      %s\n", hash);
    }

    return ok;
}
