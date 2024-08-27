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
            int64_t version;
            char *community;
            int64_t request_id;
            int64_t error;
            int64_t error_index;
            char *OID;
        } get;

        struct {
            uint8_t version;
            uint32_t request_id;
        } get_response;
    };
} packet;

typedef struct message {
    uint8_t *data;
    int N;
} message;

void packet_free(packet *);
packet *ssmp_get(int64_t version, char *community, int64_t request_id, int64_t error, int64_t error_index, char *OID);