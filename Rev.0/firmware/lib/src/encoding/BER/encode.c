#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>
#include <encoding/SSMP/SSMP.h>

int pack_null(uint8_t *buffer, int remaining);
uint8_t *reverse(uint8_t *buffer, int N);

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

message BER_encodex(const struct packet p) {
    struct message m = {
        .data = NULL,
        .length = 0,
    };

    if (p.tag == PACKET_GET_RESPONSE) {
        int N = 16;
        uint8_t *buffer = (uint8_t *)calloc(N, sizeof(uint8_t));
        int ix = 0;
        int len;

        switch (p.get_response.value.tag) {
        default:
            len = pack_null(&buffer[ix], N - ix);
            ix += len;
        }

        m.data = reverse(buffer, ix);
        m.length = ix;
    }

    return m;
}

uint8_t *reverse(uint8_t *buffer, int N) {
    uint8_t *p = &buffer[0];
    uint8_t *q = &buffer[N - 1];

    for (int i = 0; i < N / 2; i++) {
        uint8_t v = *p;

        *p++ = *q;
        *q-- = v;
    }

    return buffer;
}

int pack_null(uint8_t *buffer, int remaining) {
    if (remaining-- > 0) {
        buffer[0] = 0x01;
    }

    if (remaining-- > 0) {
        buffer[1] = 0x05;
    }

    return 2;
}