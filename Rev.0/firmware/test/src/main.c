#include <stdio.h>
#include <stdlib.h>

#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "../include/crypt/hash/hash.h"
#include "../include/encoding/ASN.1/ASN1.h"

#define _VERSION "v0.0"

const char *VERSION = _VERSION;
const uint32_t WATCHDOG_TIMEOUT = 5000; // ms

extern bool sys_init();
extern bool sys_blink();
bool validate();

typedef bool (*test)();

const test tests[] = {
    test_SHA256,
    test_ASN1_encode_integer,
    test_ASN1_decode_integer,
};

int main() {
    bi_decl(bi_program_description("uhppoted-breakout::test"));
    bi_decl(bi_program_version_string(_VERSION));

    stdio_init_all();
    sleep_ms(5000);

    // ... initialise system
    if (!sys_init()) {
        printf("ERROR INITIALISING TEST SYSTEM\n");
        return -1;
    }

    // ... validate
    if (validate()) {
        printf("*** OK\n\n");
    } else {
        printf("*** FAILED\n\n");
    }

    sleep_ms(5000);

    // ... loop 'n validate (out of memory)
    uint64_t loops = 0;
    while (true) {
        sys_blink();

        if (validate()) {
            printf("*** OK %llu\n\n", ++loops);
        } else {
            printf("*** FAILED %llu\n\n", ++loops);
        }

        sleep_ms(250);
    }
}

bool validate() {
    bool ok = true;
    int N = sizeof(tests) / sizeof(test);

    for (int i = 0; i < N; i++) {
        if (!tests[i]()) {
            ok = false;
        }

        printf("\n");
    }

    return ok;
}
