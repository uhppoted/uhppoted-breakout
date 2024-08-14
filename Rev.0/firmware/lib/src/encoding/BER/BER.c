#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

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
            .request_id = 1,
        },
    };

    return p;
}
