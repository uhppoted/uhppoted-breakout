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

    // ... done
    while (true) {
        bool ok = true;
        sys_blink();

        if (!test_SHA256()) {
            ok = false;
        }

        if (!test_ASN1_encode_integer()) {
            ok = false;
        }

        if (ok) {
            printf("\nOK\n\n");
        } else {
            printf("\n** FAILED **\n\n");
        }

        sleep_ms(2500);
    }
}
