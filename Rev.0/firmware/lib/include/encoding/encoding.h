#pragma once

#include <stdint.h>

typedef enum {
    PACKET_UNKNOWN,
    PACKET_GET,
    PACKET_GET_RESPONSE,
} PACKET;

typedef struct datetime {
} datetime;

typedef struct packet {
    PACKET tag;
    union {
        struct {
            uint32_t request_id;
        } get;

        struct {
            uint32_t request_id;
        } get_response;
    };
} packet;

typedef struct message {
    uint8_t *data;
    int N;
} message;
