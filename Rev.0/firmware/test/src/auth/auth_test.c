/*
 * Validation test suite for the auth implementation.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <auth.h>

bool test_auth_validate_seq010120123();
bool test_auth_validate_seq0656();

bool test_auth_validate() {
    bool ok = true;

    printf("-- AUTH\n");
    ok = ok && test_auth_validate_seq010120123();
    ok = ok && test_auth_validate_seq0656();

    return ok;
}

// request ID sequence [0,1,0,1,2,0,1,2,3]
bool test_auth_validate_seq010120123() {
    bool ok = true;
    bool valid;

    uint8_t key[] = {0x06, 0xed, 0x87, 0xd7, 0x95, 0xb4, 0x16, 0xbf, 0x54, 0xb9};

    auth_setkey("private", key, sizeof(key));

    // ... 0
    valid = auth_validate("private", 363589);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 0);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 1
    valid = auth_validate("private", 848788);
    printf("%-5s  counter:%d\n", valid ? "ok" : "error", 1);
    if (!valid) {
        ok = false;
        printf("       - expected: %s\n", "valid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 0
    valid = auth_validate("private", 363589);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 0);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 1
    valid = auth_validate("private", 848788);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 1);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 2
    valid = auth_validate("private", 931192);
    printf("%-5s  counter:%d\n", valid ? "ok" : "error", 2);
    if (!valid) {
        ok = false;
        printf("       - expected: %s\n", "valid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 0
    valid = auth_validate("private", 363589);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 0);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 1
    valid = auth_validate("private", 848788);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 1);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 2
    valid = auth_validate("private", 931192);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 2);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 3
    valid = auth_validate("private", 38120);
    printf("%-5s  counter:%d\n", valid ? "ok" : "error", 3);
    if (!valid) {
        ok = false;
        printf("       - expected: %s\n", "valid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    return ok;
}

// request ID sequence [0,6,5,6]
bool test_auth_validate_seq0656() {
    bool ok = true;
    bool valid;

    uint8_t key[] = {0x06, 0xed, 0x87, 0xd7, 0x95, 0xb4, 0x16, 0xbf, 0x54, 0xb9};

    auth_setkey("private", key, sizeof(key));

    // ... 0
    valid = auth_validate("private", 363589);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 0);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 6
    valid = auth_validate("private", 280652);
    printf("%-5s  counter:%d\n", !valid ? "ok" : "error", 6);
    if (valid) {
        ok = false;
        printf("       - expected: %s\n", "invalid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 5
    valid = auth_validate("private", 797611);
    printf("%-5s  counter:%d\n", valid ? "ok" : "error", 5);
    if (!valid) {
        ok = false;
        printf("       - expected: %s\n", "valid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    // ... 6
    valid = auth_validate("private", 280652);
    printf("%-5s  counter:%d\n", valid ? "ok" : "error", 6);
    if (!valid) {
        ok = false;
        printf("       - expected: %s\n", "valid");
        printf("       - got:      %s\n", valid ? "valid" : "invalid");
    }

    return ok;
}
