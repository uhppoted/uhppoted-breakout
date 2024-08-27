#pragma once

#include <stdint.h>

typedef enum {
    PACKET_UNKNOWN,
    PACKET_GET,
    PACKET_GET_RESPONSE,
} PACKET;

typedef enum {
    VALUE_UNKNOWN,
    VALUE_UINT32
} VALUE;

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
            char *community;
            uint32_t request_id;
            int64_t error;
            int64_t error_index;
            char *OID;
            struct {
                VALUE tag;
                union {
                    int64_t integer;
                };
            } value;
        } get_response;
    };
} packet;

typedef struct message {
    uint8_t *data;
    int length;
} message;

void packet_free(packet *);
packet *ssmp_get(int64_t version, char *community, int64_t request_id, int64_t error, int64_t error_index, char *OID);