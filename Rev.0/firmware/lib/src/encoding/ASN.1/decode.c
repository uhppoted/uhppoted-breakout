#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <encoding/SSMP/SSMP.h>

vector *unpack(const uint8_t *bytes, int N);

// clang-format off
field *unpack_integer (const uint8_t *, int, int *);
field *unpack_octets  (const uint8_t *, int, int *);
field *unpack_null    (const uint8_t *, int, int *);
field *unpack_OID     (const uint8_t *, int, int *);
field *unpack_sequence(const uint8_t *, int, int *);
field *unpack_get_request    (const uint8_t *, int, int *);
// clang-format on

uint32_t unpack_length(const uint8_t *message, int N, int *ix);

struct packet *BER_decode(const uint8_t *message, int N) {
    packet *p = NULL;

    // ... unpack message
    vector *fields = unpack(message, N);

    if (fields != NULL) {
        if (fields->size > 0 && fields->fields[0] != NULL && fields->fields[0]->tag == FIELD_SEQUENCE && fields->fields[0]->sequence.fields != NULL) {
            vector message = *fields->fields[0]->sequence.fields;

            // ... SSMP GET request ?
            if (message.size > 2 && message.fields[2]->tag == FIELD_PDU_GET) {
                int64_t version = 0;
                char *community = NULL;
                int64_t request_id = 0;
                int64_t error = 0;
                int64_t error_index = 0;
                char *OID = NULL;

                if (message.fields[0]->tag == FIELD_INTEGER) {
                    version = message.fields[0]->integer.value;
                }

                if (message.fields[1]->tag == FIELD_OCTET_STRING) {
                    community = strndup(message.fields[1]->octets.octets, message.fields[1]->octets.length);
                }

                if (message.fields[2]->sequence.fields != NULL) {
                    vector pdu = *message.fields[2]->sequence.fields;

                    if (pdu.size > 0 && pdu.fields[0]->tag == FIELD_INTEGER) {
                        request_id = pdu.fields[0]->integer.value;
                    }

                    if (pdu.size > 1 && pdu.fields[1]->tag == FIELD_INTEGER) {
                        error = pdu.fields[1]->integer.value;
                    }

                    if (pdu.size > 2 && pdu.fields[2]->tag == FIELD_INTEGER) {
                        error_index = pdu.fields[2]->integer.value;
                    }

                    if (pdu.size > 3 && pdu.fields[3]->tag == FIELD_SEQUENCE && pdu.fields[3]->sequence.fields != NULL) {
                        vector content = *pdu.fields[3]->sequence.fields;

                        if (content.size > 0 && content.fields[0]->tag == FIELD_SEQUENCE && content.fields[0]->sequence.fields != NULL) {
                            vector rq = *content.fields[0]->sequence.fields;

                            if (rq.size > 0 && rq.fields[0]->tag == FIELD_OID) {
                                OID = strdup(rq.fields[0]->OID.OID);
                            }

                            if (rq.size > 1 && rq.fields[1]->tag == FIELD_NULL) {
                            }
                        }
                    }
                }

                p = ssmp_get(version, community, request_id, error, error_index, OID);
            }
        }

        vector_free(fields);
        free(fields);
    }

    return p;
}

vector *unpack(const uint8_t *bytes, int N) {
    vector *v = vector_new();

    if (v != NULL) {
        int ix = 0;
        int count = 0;
        field *f;

        while (ix < N && ++count < 16) {
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

            case FIELD_OID:
                if ((f = unpack_OID(bytes, N, &ix)) != NULL) {
                    v = vector_add(v, f);
                }
                break;

            case FIELD_SEQUENCE:
                if ((f = unpack_sequence(bytes, N, &ix)) != NULL) {
                    v = vector_add(v, f);
                }
                break;

            case FIELD_PDU_GET:
                if ((f = unpack_get_request(bytes, N, &ix)) != NULL) {
                    v = vector_add(v, f);
                }
                break;

            default:
                // printf("::unknown:%2d  N:%d  ix:%d\n", tag, N, ix);
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
            value <<= 8;
            value |= message[*ix + i] & 0x00ff;
        }
    }

    // printf("::integer     N:%d  ix:%-3d length:%lu  value:%lld\n", N, *ix, length, value);

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

    // printf("::octets      N:%d  ix:%-3d length:%lu  octets:%s\n", N, *ix, length, strndup(octets, length));

    // ... compose field
    field *f = (field *)calloc(1, sizeof(field));
    f->tag = FIELD_OCTET_STRING;
    f->octets.length = length;
    f->octets.octets = octets;

    *ix += length;

    return f;
}

field *unpack_null(const uint8_t *message, int N, int *ix) {
    uint32_t length = unpack_length(message, N, ix);

    // printf("::null        N:%d  ix:%-3d length:%lu\n", N, *ix, length);

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
