/*
 * Validation test suite for HOTP implementation.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <crypt/otp/hotp.h>

extern uint32_t hotp_generate(const uint8_t *secret, int length, uint64_t counter);

bool test_HOTP() {
    bool ok = true;

    printf("-- HOTP\n");

    uint8_t secret[] = {0x06, 0xed, 0x87, 0xd7, 0x95, 0xb4, 0x16, 0xbf, 0x54, 0xb9};

    uint64_t counters[] = {
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        405419896,
    };

    uint32_t codes[] = {
        363589, // 0
        848788, // 1
        931192, // 2
        38120,  // 3
        881319, // 4
        797611, // 5
        280652, // 6
        442564, // 7
        768861, // 8
        947109, // 9
        526974, // 10
        435988, // 1000
        830127, // 10000
        224802, // 100000
        902124, // 1000000
        256869, // 10000000
        718655, // 100000000
        244053, // 405419896
    };

    int N = sizeof(counters) / sizeof(uint64_t);

    for (int i = 0; i < N; i++) {
        uint32_t code = hotp_generate(secret, sizeof(secret), counters[i]);

        printf("%-5s  counter:%d\n", code == codes[i] ? "ok" : "error", counters[i]);
        if (code != codes[i]) {
            ok = false;
            printf("       - expected: %06lu\n", codes[i]);
            printf("       - got:      %06lu\n", code);
        }
    }

    return ok;
}
