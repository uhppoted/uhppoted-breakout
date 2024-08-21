#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <encoding/encoding.h>

struct message BER_encode(const struct packet p);
struct packet BER_decode(const uint8_t *message, int N);

/* internal use only */

typedef enum {
    FIELD_UNKNOWN = 0x00,
    FIELD_BOOLEAN = 0x01,
    FIELD_INTEGER = 0x02,
    FIELD_OCTET_STRING = 0x04,
    FIELD_NULL = 0x05,
    FIELD_OID = 0x06,
    FIELD_SEQUENCE = 0x30,
    FIELD_PDU = 0xa0,
} FIELD;

typedef struct field field;
typedef field *fieldp;
typedef fieldp *fields;

typedef struct vector {
    int N;
    field *fields[];
} vector;

typedef struct field {
    FIELD tag;
    union {
        struct {
            int64_t value;
        } integer;

        struct {
            int length;
            uint8_t *octets;
        } string;

        struct {
        } null;

        struct {
            char *OID;
        } OID;

        struct {
            fields fields;
        } sequence;

        struct {
            fields fields;
        } PDU;
    };

} field;
