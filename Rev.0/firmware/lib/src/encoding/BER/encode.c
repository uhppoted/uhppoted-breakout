#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>
#include <encoding/SSMP/SSMP.h>

slice pack_integer(const field *f);
slice pack_null(const field *f);
slice pack_OID(const field *f);

// uint8_t *reverse(uint8_t *buffer, int N);

// clang-format off
const uint8_t RESPONSE[] = {
    48,  41,  2,   1,   0,  4,   6,  112, 
    117, 98,  108, 105, 99, 162, 28, 2, 
    1,   1,   2,   1,   0,  2,   1,  0,
    48,  17,  48,  15,  6,  7,   43, 6,
    167, 254, 32,  1,   1,  71,  4,  24, 
    42,  55,  120};
// clang-format on

message BER_encode(const struct packet p) {
    struct message m = {
        .data = (uint8_t *)calloc(sizeof(RESPONSE), sizeof(uint8_t)),
        .length = sizeof(RESPONSE),
    };

    memmove(m.data, RESPONSE, m.length);

    return m;
}

slice BER_encodex(const struct field f) {
    struct slice s = {
        .length = 0,
        .bytes = NULL,
    };

    switch (f.tag) {
    case FIELD_INTEGER:
        s = pack_integer(&f);
        break;

    case FIELD_NULL:
        s = pack_null(&f);
        break;

    case FIELD_OID:
        s = pack_OID(&f);
        break;
    }

    return s;
}

// uint8_t *reverse(uint8_t *buffer, int N) {
//     uint8_t *p = &buffer[0];
//     uint8_t *q = &buffer[N - 1];
//
//     for (int i = 0; i < N / 2; i++) {
//         uint8_t v = *p;
//
//         *p++ = *q;
//         *q-- = v;
//     }
//
//     return buffer;
// }

slice pack_integer(const field *f) {
    uint8_t buffer[8] = {0};
    int ix = 0;
    int64_t v = f->integer.value;

    do {
        buffer[ix++] = v & 0x00ff;
        v >>= 8;
    } while (v != 0 && ix < sizeof(buffer));

    slice s = {
        .length = ix + 2,
        .bytes = (uint8_t *)calloc(ix + 2, sizeof(uint8_t)),
    };

    s.bytes[0] = 0x02;
    s.bytes[1] = (uint8_t)ix;
    for (int i = 0; i < ix; i++) {
        s.bytes[i + 2] = buffer[ix - i - 1];
    }

    return s;
}

slice pack_null(const field *f) {
    slice s = {
        .length = 2,
        .bytes = (uint8_t *)calloc(2, sizeof(uint8_t)),
    };

    s.bytes[0] = 0x05;
    s.bytes[1] = 0x00;

    return s;
}

slice pack_OID(const field *f) {
    char *oid = strdup(f->OID.OID);
    char *saveptr;
    char *p = strtok_r(oid, ".", &saveptr);
    uint32_t first = 0x00;
    int len = 0;

    // ... skip leading '0'
    if (p != NULL) {
        uint32_t v = 0;

        if (sscanf(p, "%lu", &v) == 1) {
            printf(">>> 1::%s  [%lu]\n", p, v);
        }

        p = strtok_r(NULL, ".", &saveptr);
    }

    // ... special encoding for first byte (.1.3)
    if (p != NULL) {
        uint32_t v = 0;

        if (sscanf(p, "%lu", &v) == 1) {
            printf(">>> 2::%s  [%lu]\n", p, v);
            first = 40 * v;
        }

        if ((p = strtok_r(NULL, ".", &saveptr)) p != NULL) {
            uint32_t v = 0;

            if (sscanf(p, "%lu", &v) == 1) {
                printf(">>> 3::%s  [%lu]\n", p, v);
                first += v;
            }
        }

        p = strtok_r(NULL, ".", &saveptr);

        len++;
    }

    // while (p != NULL) {
    //     printf(">> %s\n", p);
    //     p = strtok_r(NULL, ".", &saveptr);
    // };

    slice s = {
        .length = 2 + len,
        .bytes = (uint8_t *)calloc(2 + len, sizeof(uint8_t)),
    };

    s.bytes[0] = 0x06;
    s.bytes[1] = len;

    if (len > 0) {
        s.bytes[2] = (uint8_t)first;
    }

    free(oid);

    return s;
}
