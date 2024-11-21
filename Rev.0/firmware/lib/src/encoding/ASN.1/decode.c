#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <log.h>

// clang-format off
field *unpack_integer    (const uint8_t *, int, int *);
field *unpack_octets     (const uint8_t *, int, int *);
field *unpack_null       (const uint8_t *, int, int *);
field *unpack_OID        (const uint8_t *, int, int *);
field *unpack_sequence   (const uint8_t *, int, int *);
field *unpack_get_request(const uint8_t *, int, int *);
// clang-format on

const int MAX_FIELDS = 16;

vector *unpack(const uint8_t *bytes, int N);
uint32_t unpack_length(const uint8_t *message, int N, int *ix);

vector *BER_decode(const uint8_t *message, int N) {
    return unpack(message, N);
}

vector *unpack(const uint8_t *bytes, int N) {
    vector *v = vector_new();

    if (v != NULL) {
        int ix = 0;
        int count = 0;
        field *f;

        while (ix < N && ++count < MAX_FIELDS) {
            uint8_t tag = bytes[ix++];

            switch (tag) {
            case FIELD_INTEGER:
                if ((f = unpack_integer(bytes, N, &ix)) != NULL) {
                    v = vector_add(v, f);
                }
                break;

            case FIELD_OCTET_STRING:
                if ((f = unpack_octets(bytes, N, &ix)) != NULL) {
                    v = vector_add(v, f);
                }
                break;

            case FIELD_NULL:
                if ((f = unpack_null(bytes, N, &ix)) != NULL) {
                    v = vector_add(v, f);
                }
                break;

                //         case FIELD_OID:
                //             if ((f = unpack_OID(bytes, N, &ix)) != NULL) {
                //                 // v = vector_add(v, f);
                //                 // field_free(f);
                //             }
                //             break;

                //         case FIELD_SEQUENCE:
                //             if ((f = unpack_sequence(bytes, N, &ix)) != NULL) {
                //                 // v = vector_add(v, f);
                //                 // field_free(f);
                //             }
                //             break;

                //         case FIELD_PDU_GET:
                //             if ((f = unpack_get_request(bytes, N, &ix)) != NULL) {
                //                 // v = vector_add(v, f);
                //                 // field_free(f);
                //             }
                //             break;

            default:
                debugf("ASN.1", "decode::unknown:%2d  N:%d  ix:%d\n", tag, N, ix);
                ix = N;
            }
        }
    }

    return v;
}

field *unpack_integer(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);
    int64_t value = 0;

    if (length <= 8) {
        for (int i = 0; i < length; i++) {
            uint8_t u8 = message[*ix + i];
            value = ((u8 & 0x80) == 0x80) ? -1 : 0;
            value <<= 8;
            value |= message[*ix + i] & 0x00ff;
            break;
        }

        for (int i = 1; i < length; i++) {
            value <<= 8;
            value |= message[*ix + i] & 0x00ff;
        }
    }

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));

    f->dynamic = true;
    f->tag = FIELD_INTEGER;
    f->integer.value = value;

    *ix += length;

    return f;
}

field *unpack_octets(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);
    uint8_t *octets = (uint8_t *)calloc(length, sizeof(uint8_t));

    memmove(octets, &message[*ix], length);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));

    f->dynamic = true;
    f->tag = FIELD_OCTET_STRING;
    f->octets.length = length;
    f->octets.octets = octets;

    *ix += length;

    return f;
}

field *unpack_null(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));

    f->tag = FIELD_NULL;
    f->dynamic = true;

    *ix += length;

    return f;
}

field *unpack_OID(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);
    char *OID = (char *)calloc(256, sizeof(char));
    char *p = OID;
    char *end = p + 256;

    p += snprintf(p, end - p, "0");

    if (length > 0 && p < end) {
        p += snprintf(p, end - p, ".%d", message[*ix] / 40);
    }

    if (length > 0 && p < end) {
        p += snprintf(p, end - p, ".%d", message[*ix] % 40);
    }

    int i = 1;
    while (i < length && p < end) {
        uint64_t suboid = 0;

        while (i < length && p < end) {
            uint8_t b = message[*ix + i++];

            suboid <<= 7;
            suboid += (uint64_t)(b & 0x7f);

            if ((b & 0x80) == 0x00) {
                p += snprintf(p, end - p, ".%llu", suboid);
                break;
            }
        }
    }

    // printf("::OID         N:%d  ix:%-3d length:%lu  OID:%s\n", N, *ix, length, OID);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_OID;
    f->OID.OID = OID;

    *ix += length;

    return f;
}

field *unpack_sequence(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);
    vector *fields = unpack(&message[*ix], length);

    // printf("::sequence    N:%d  ix:%-3d length:%lu  fields:%d\n", N, *ix, length, fields->size);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_SEQUENCE;
    f->sequence.fields = fields;

    *ix += length;

    return f;
}

field *unpack_get_request(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);
    vector *fields = unpack(&message[*ix], length);

    // printf("::PDU         N:%d  ix:%-3d length:%lu  fields:%d\n", N, *ix, length, fields->size);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_PDU_GET;
    f->pdu.fields = fields;

    *ix += length;

    return f;
}

uint32_t unpack_length(const uint8_t *message, int N, int *ix) {
    uint32_t length = 0;

    if (*ix < N) {
        uint8_t b = message[*ix];
        *ix += 1;

        if ((b & 0x80) == 0x00) {
            length = (uint32_t)(b & 0x7f);
        } else {
            int len = b & 0x7f;
            for (int i = 0; i < len && *ix < N; i++) {
                length <<= 8;
                length += (uint32_t)(message[*ix]);
                *ix += 1;
            }
        }
    }

    return length;
}
