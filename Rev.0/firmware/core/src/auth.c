#include <stdlib.h>
#include <string.h>

#include <MIB.h>
#include <auth.h>
#include <crypt/otp/hotp.h>

typedef struct community {
    uint64_t counter;
    size_t keysize;
    uint8_t *key;
} community;

const struct {
    char *get[18];
    char *set[1];
} PUBLIC = {
    .get = {
        MIB_CONTROLLER_ID,
        MIB_CONTROLLER_VERSION,
        MIB_CONTROLLER_RELEASED,
        MIB_CONTROLLER_DATETIME,
        MIB_CONTROLLER_SYSERROR,
        MIB_CONTROLLER_SYSINFO,

        MIB_DOORS_1_UNLOCKED,
        MIB_DOORS_2_UNLOCKED,
        MIB_DOORS_3_UNLOCKED,
        MIB_DOORS_4_UNLOCKED,

        MIB_DOORS_1_OPEN,
        MIB_DOORS_2_OPEN,
        MIB_DOORS_3_OPEN,
        MIB_DOORS_4_OPEN,

        MIB_DOORS_1_BUTTON,
        MIB_DOORS_2_BUTTON,
        MIB_DOORS_3_BUTTON,
        MIB_DOORS_4_BUTTON,
    },

    .set = {
        MIB_CONTROLLER_DATETIME,
    },
};

community public = {
    .counter = 0,
    .keysize = 0,
    .key = NULL,
};

community private = {
    .counter = 0,
    .keysize = 0,
    .key = NULL,
};

bool auth_authorised(const char *community, const char *oid, OP op) {
    if (strcmp(community, "public") == 0 && op == OP_GET) {
        int N = sizeof(PUBLIC.get) / sizeof(const char *);
        for (int i = 0; i < N; i++) {
            if (strcmp(oid, PUBLIC.get[i]) == 0) {
                return true;
            }
        }
    }

    if (strcmp(community, "public") == 0 && op == OP_SET) {
        int N = sizeof(PUBLIC.set) / sizeof(const char *);
        for (int i = 0; i < N; i++) {
            if (strcmp(oid, PUBLIC.set[i]) == 0) {
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

    if (strcmp(community, "private") == 0 && private.key != NULL && private.keysize > 0) {
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

void auth_setkey(const char *community, const uint8_t *key, size_t length) {
    if (strcmp(community, "private") == 0) {
        uint8_t *oldkey = private.key;
        uint8_t *newkey = NULL;
        size_t N = length > 0 && key != NULL ? length : 0;

        if (key != NULL && N > 0) {
            newkey = (uint8_t *)calloc(sizeof(uint8_t), N);

            memmove(newkey, key, length);
        }

        private.counter = 0;
        private.key = newkey;
        private.keysize = N;

        free(oldkey);
    }
}