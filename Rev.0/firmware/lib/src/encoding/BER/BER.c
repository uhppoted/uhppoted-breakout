#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

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

    // ... unpack message
    int ix = 0;

    if (ix < N) {
        uint8_t tag = message[ix++];

        switch (tag) {
        case 0x30:
            unpack_sequence(message, N, &ix);
            break;
        }
    }

    fflush(stdout);

    return p;
}

void unpack_sequence(const uint8_t *message, int N, int *ix) {
    // ... length
    uint64_t length = 0;
    if (*ix < N) {
        uint8_t b = message[*ix++];

        if (b & 0x80 == 0x80) {
            length = (uint64_t)(b & 0x7f);
        } else {
            int len = b & 0x7f;
            for (int i = 0; i < len && *ix < N; i++) {
                length <<= 8;
                length += (uint64_t)(message[*ix++]);
            }
        }
    }

    printf("<<sequence %llu>>   ", length);
    fflush(stdout);
}
