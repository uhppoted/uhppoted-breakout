#pragma once

#include <stdint.h>

#include <types/fields.h>
#include <types/slice.h>

typedef enum {
    PACKET_UNKNOWN,
    PACKET_GET,
    PACKET_SET,
    PACKET_RESPONSE,
    PACKET_TRAP,
} PACKET;

typedef enum {
    VALUE_UNKNOWN,
    VALUE_BOOLEAN,
    VALUE_UINT8,
    VALUE_UINT16,
    VALUE_UINT32,
    VALUE_OCTET_STRING,
    VALUE_NULL,
} VALUE;

typedef struct value {
    VALUE tag;

    union {
        bool boolean;
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
        } set;

        struct {
            uint32_t request_id;
            int64_t error;
            int64_t error_index;
            char *OID;
            value value;
        } response;

        struct {
            char *OID;
            uint32_t id;
            uint32_t category;
            uint32_t event;
            uint32_t timestamp;
            value value; // FIXME allow multiple values
        } trap;
    };

    bool dynamic;
} packet;

void packet_free(packet *);
