#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

void unpack(const uint8_t *message, int N, int *ix);
void unpack_boolean(const uint8_t *message, int N, int *ix);
void unpack_null(const uint8_t *message, int N, int *ix);
void unpack_OID(const uint8_t *message, int N, int *ix);
void unpack_sequence(const uint8_t *message, int N, int *ix);

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
        .N = sizeof(RESPONSE),
    };

    memmove(m.data, RESPONSE, m.N);

    return m;
}

struct packet BER_decode(const uint8_t *message, int N) {
    packet p = {
        .tag = PACKET_GET,
        .get = {
            .version = 0,
            .request_id = 1,
        },
    };

    // // ... unpack message
    // int ix = 0;
    //
    // if (ix < N) {
    //     uint8_t tag = message[ix++];
    //
    //     switch (tag) {
    //     case 0x30:
    //         unpack_sequence(message, N, &ix);
    //         break;
    //     }
    // }

    int ix = 0;
    unpack(message, N, &ix);

    return p;
}

void unpack(const uint8_t *bytes, int N, int *ix) {
    int fields = 0;

    while (*ix < N && ++fields < 10) {
        uint8_t tag = bytes[*ix];
        *ix += 1;

        switch (tag) {
        case 1:
            unpack_boolean(bytes, N, ix);
            break;

        case 5:
            unpack_null(bytes, N, ix);
            break;

        case 6:
            unpack_OID(bytes, N, ix);
            break;

        case 48:
            unpack_sequence(bytes, N, ix);
            break;

        default:
            printf("::unknown:%2d  N:%d  ix:%d\n", tag, N, *ix);
            *ix = N;
        }
    }

    fflush(stdout);
}

void unpack_boolean(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 1;
    // if (*ix < N) {
    //     uint8_t b = message[*ix];
    //     *ix += 1;

    //     if (b & 0x80 == 0x80) {
    //         length = (uint32_t)(b & 0x7f);
    //     } else {
    //         int len = b & 0x7f;
    //         for (int i = 0; i < len && *ix < N; i++) {
    //             length <<= 8;
    //             length += (uint32_t)(message[*ix]);
    //             *ix += 1;
    //         }
    //     }
    // }

    printf("::boolean     N:%d  ix:%d  length:%lu  end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;
}

void unpack_null(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 1;
    // if (*ix < N) {
    //     uint8_t b = message[*ix];
    //     *ix += 1;

    //     if (b & 0x80 == 0x80) {
    //         length = (uint32_t)(b & 0x7f);
    //     } else {
    //         int len = b & 0x7f;
    //         for (int i = 0; i < len && *ix < N; i++) {
    //             length <<= 8;
    //             length += (uint32_t)(message[*ix]);
    //             *ix += 1;
    //         }
    //     }
    // }

    printf("::null        N:%d  ix:%d  length:%lu  end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;
}

void unpack_OID(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 0;
    if (*ix < N) {
        uint8_t b = message[*ix];
        *ix += 1;

        if (b & 0x80 == 0x80) {
            length = (uint32_t)(b & 0x7f);
        } else {
            int len = b & 0x7f;
            for (int i = 0; i < len && *ix < N; i++) {
                length <<= 8;
                length += (uint32_t)(message[*ix]);
                *ix += 1;
            }
        }
    }

    printf("::OID         N:%d  ix:%d  length:%lu  end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;
}

void unpack_sequence(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 0;
    if (*ix < N) {
        uint8_t b = message[*ix];
        *ix += 1;

        if (b & 0x80 == 0x80) {
            length = (uint32_t)(b & 0x7f);
        } else {
            int len = b & 0x7f;
            for (int i = 0; i < len && *ix < N; i++) {
                length <<= 8;
                length += (uint32_t)(message[*ix]);
                *ix += 1;
            }
        }
    }

    printf("::sequence    N:%d  ix:%d  length:%lu end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;
}
