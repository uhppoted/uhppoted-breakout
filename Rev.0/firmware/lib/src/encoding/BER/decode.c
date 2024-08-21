#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

extern void fields_free(fields);
extern fields fields_add(fields, field *);

fields unpack(const uint8_t *message, int N);

// clang-format off
field *unpack_integer (const uint8_t *, int, int *);
field *unpack_octets  (const uint8_t *, int, int *);
field *unpack_null    (const uint8_t *, int, int *);
field *unpack_OID     (const uint8_t *, int, int *);
field *unpack_sequence(const uint8_t *, int, int *);
field *unpack_PDU     (const uint8_t *, int, int *);
// clang-format on

uint32_t unpack_length(const uint8_t *message, int N, int *ix);

struct packet BER_decode(const uint8_t *message, int N) {
    packet p = {
        .tag = PACKET_GET,
        .get = {
            .version = 0,
            .request_id = 1,
        },
    };

    // ... unpack message
    fields fields = unpack(message, N);

    if (fields != NULL) {
        fields_free(fields);
    }

    return p;
}

fields unpack(const uint8_t *bytes, int N) {
    fields list;
    field *f;
    int length = 0;
    int ix = 0;

    if ((list = calloc(1, sizeof(fieldp))) == NULL) {
        return NULL;
    } else {
        list[0] = NULL;
    }

    while (ix < N && ++length < 16) {
        uint8_t tag = bytes[ix];
        ix += 1;

        switch (tag) {
        case FIELD_INTEGER:
            if ((f = unpack_integer(bytes, N, &ix)) != NULL) {
                list = fields_add(list, f);
            }
            break;

        case FIELD_OCTET_STRING:
            if ((f = unpack_octets(bytes, N, &ix)) != NULL) {
                list = fields_add(list, f);
            }
            break;

        case FIELD_NULL:
            if ((f = unpack_null(bytes, N, &ix)) != NULL) {
                list = fields_add(list, f);
            }
            break;

        case FIELD_OID:
            if ((f = unpack_OID(bytes, N, &ix)) != NULL) {
                list = fields_add(list, f);
            }
            break;

        case FIELD_SEQUENCE:
            if ((f = unpack_sequence(bytes, N, &ix)) != NULL) {
                list = fields_add(list, f);
            }
            break;

        case FIELD_PDU:
            if ((f = unpack_PDU(bytes, N, &ix)) != NULL) {
                list = fields_add(list, f);
            }
            break;

        default:
            printf("::unknown:%2d  N:%d  ix:%d\n", tag, N, ix);
            ix = N;
        }
    }

    fflush(stdout);

    return list;
}

field *unpack_integer(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);
    int64_t value = 0;

    if (length <= 8) {
        for (int i = 0; i < length; i++) {
            value <<= 8;
            value |= message[*ix + i] & 0x00ff;
        }
    }

    printf("::integer     N:%d  ix:%-3d length:%lu  value:%lld\n", N, *ix, length, value);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_INTEGER;
    f->integer.value = value;

    *ix += length;

    return f;
}

field *unpack_octets(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);
    uint8_t *octets = (uint8_t *)calloc(length, sizeof(uint8_t));

    memmove(octets, &message[*ix], length);

    printf("::octets      N:%d  ix:%-3d length:%lu  octets:%s\n", N, *ix, length, strndup(octets, length));

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_OCTET_STRING;
    f->string.length = length;
    f->string.octets = octets;

    *ix += length;

    return f;
}

field *unpack_null(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);

    printf("::null        N:%d  ix:%-3d length:%lu\n", N, *ix, length);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_NULL;

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

    printf("::OID         N:%d  ix:%-3d length:%lu  OID:%s\n", N, *ix, length, OID);

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_OID;
    f->OID.OID = OID;

    *ix += length;

    return f;
}

field *unpack_sequence(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);

    printf("::sequence    N:%d  ix:%-3d length:%lu\n", N, *ix, length);

    fields list = unpack(&message[*ix], length);

    printf("::sequence/fields %p\n", list);
    fields_free(list);

    // ... 'k, done'
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_SEQUENCE;
    f->sequence.fields = NULL;

    *ix += length;

    return f;
}

field *unpack_PDU(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);

    printf("::PDU         N:%d  ix:%-3d length:%lu\n", N, *ix, length);

    fields list = unpack(&message[*ix], length);
    fields_free(list);

    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_PDU;
    f->PDU.fields = NULL;

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
