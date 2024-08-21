#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <encoding/encoding.h>

struct message BER_encode(const struct packet p);
struct packet BER_decode(const uint8_t *message, int N);

/* internal use only */

typedef enum {
    FIELD_UNKNOWN = 0,
    FIELD_BOOLEAN = 1,
    FIELD_INTEGER = 2,
    FIELD_NULL = 5,
    FIELD_OID = 6,
    FIELD_SEQUENCE = 48,
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
            fields fields;
        } sequence;
    };

} field;
