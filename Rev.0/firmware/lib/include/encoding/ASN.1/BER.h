#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <types/slice.h>
#include <types/vector.h>

struct packet;
struct field;
struct vector;

struct slice BER_encode(const struct field f);
struct vector *BER_decode(const uint8_t *message, int N);

extern void field_free(struct field *const);

extern struct vector *vector_new();
extern void vector_free(struct vector *const);
extern struct vector *vector_add(struct vector *, struct field *);

/* internal use only */

typedef enum {
    FIELD_UNKNOWN = 0x00,
    FIELD_BOOLEAN = 0x01,
    FIELD_INTEGER = 0x02,
    FIELD_OCTET_STRING = 0x04,
    FIELD_NULL = 0x05,
    FIELD_OID = 0x06,
    FIELD_SEQUENCE = 0x30,
    FIELD_PDU_GET = 0xa0,
    // FIELD_PDU_GET_NEXT = 0xA1,
    FIELD_PDU_GET_RESPONSE = 0xa2,
    // FIELD_PDU_SET: 0xA3,
    // FIELD_PDU_TRAP: 0xA4,
} FIELD;

typedef struct field {
    FIELD tag;
    union {
        struct {
            int64_t value;
        } integer;

        struct {
            int length;
            uint8_t *octets;
        } octets;

        struct {
        } null;

        struct {
            char *OID;
        } OID;

        struct {
            vector *fields;
        } sequence;

        struct {
            vector *fields;
        } pdu;
    };

} field;
