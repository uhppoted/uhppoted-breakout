#include <stdio.h>
#include <string.h>

#include <auth.h>
#include <crypt/otp/hotp.h>

typedef struct community {
    uint64_t counter;
    size_t keysize;
    uint8_t key[];
} community;

const char *PUBLIC[] = {
    "0.1.3.6.1.4.1.65536.1.1",
};

community public = {
    .counter = 0,
    .keysize = 0,
    .key = {},
};

community private = {
    .counter = 0,
    .keysize = 10,
    .key = {0x06, 0xed, 0x87, 0xd7, 0x95, 0xb4, 0x16, 0xbf, 0x54, 0xb9},
};

bool auth_authorised(const char *community, const char *oid) {
    if (strcmp(community, "public") == 0) {
        int N = sizeof(PUBLIC) / sizeof(const char *);

        for (int i = 0; i < N; i++) {
            if (strcmp(oid, PUBLIC[i]) == 0) {
                return true;
            }
        }
    }

    if (strcmp(community, "private") == 0) {
        return true;
    }

    return false;
}

bool auth_validate(const char *community, uint32_t code) {
    if (strcmp(community, "public") == 0) {
        return true;
    }

    if (strcmp(community, "private") == 0) {
        uint64_t counter = private.counter;
        for (int i = 0; i < 5; i++) {
            counter++;

            if (hotp_validate(private.key, private.keysize, counter, code)) {
                private.counter = counter;
                return true;
            }
        }

        return false;
    }

    return false;
}
