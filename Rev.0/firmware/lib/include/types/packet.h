#pragma once

#include <stdint.h>

#include <types/slice.h>
#include <types/vector.h>

typedef enum {
    PACKET_UNKNOWN,
    PACKET_GET,
    PACKET_GET_RESPONSE,
} PACKET;

typedef enum {
    VALUE_UNKNOWN,
    VALUE_UINT16,
    VALUE_UINT32,
    VALUE_OCTET_STRING,
    VALUE_NULL,
} VALUE;

typedef struct value {
    VALUE tag;

    union {
        int64_t integer;
        slice octets;
    };
} value;

typedef struct packet {
    PACKET tag;
    int64_t version;
    char *community;
    union {
        struct {
            int64_t request_id;
            int64_t error;
            int64_t error_index;
            char *OID;
        } get;

        struct {
            uint32_t request_id;
            int64_t error;
            int64_t error_index;
            char *OID;
            value value;
        } get_response;
    };

    bool dynamic;
} packet;

void free_packet(packet *);