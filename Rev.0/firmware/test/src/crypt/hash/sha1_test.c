/*
 * Validation test suite for SHA-1 implementation.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <crypt/hash/sha1.h>

bool test_SHA1() {
    bool ok = false;

    printf("-- SHA-1\n");

    // ... calculate hash
    char *expected = "a9993e364706816aba3e25717850c26c9cd0d89d";
    sha1 ctx;

    sha1_init(&ctx);
    sha1_update(&ctx, "abc", 3);
    sha1_finalize(&ctx);

    // ... convert to hex
    char hash[65];
    int ix = 0;

    for (int i = 0; i < 20; i++) {
        ix += snprintf(&hash[ix], sizeof(hash) - ix, "%02x", ctx.hash.b[i]);
    }

    // ... print result
    if (strcmp(hash, expected) == 0) {
        ok = true;
    }

    printf("%-5s  %s\n", ok ? "ok" : "error", "FIPS 180-2 C.1");
    if (!ok) {
        printf("       - expected: %s\n", expected);
        printf("       - got:      %s\n", hash);
    }

    return ok;
}
