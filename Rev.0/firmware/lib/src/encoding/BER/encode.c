#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>
#include <encoding/SSMP/SSMP.h>

slice pack_integer(const field *f);
slice pack_null(const field *f);
slice pack_OID(const field *f);
slice pack_sequence(const field *f);
slice pack_varint(const uint32_t length);

// clang-format off
const uint8_t RESPONSE[] = {
    48,  41,  2,   1,   0,  4,   6,  112, 
    117, 98,  108, 105, 99, 162, 28, 2, 
    1,   1,   2,   1,   0,  2,   1,  0,
    48,  17,  48,  15,  6,  7,   43, 6,
    167, 254, 32,  1,   1,  71,  4,  24, 
    42,  55,  120};
// clang-format on

message BER_encodex(const struct packet p) {
    struct message m = {
        .data = (uint8_t *)calloc(sizeof(RESPONSE), sizeof(uint8_t)),
        .length = sizeof(RESPONSE),
    };

    memmove(m.data, RESPONSE, m.length);

    return m;
}

slice BER_encode(const struct field f) {
    struct slice s = {
        .capacity = 0,
        .length = 0,
        .bytes = NULL,
    };

    switch (f.tag) {
    case FIELD_INTEGER:
        s = pack_integer(&f);
        break;

    case FIELD_NULL:
        s = pack_null(&f);
        break;

    case FIELD_OID:
        s = pack_OID(&f);
        break;

    case FIELD_SEQUENCE:
        s = pack_sequence(&f);
        break;
    }

    return s;
}

slice pack_integer(const field *f) {
    uint8_t buffer[8] = {0};
    int ix = 0;
    int64_t v = f->integer.value;

    do {
        buffer[ix++] = v & 0x00ff;
        v >>= 8;
    } while (v != 0 && ix < sizeof(buffer));

    slice s = {
        .capacity = ix + 2,
        .length = ix + 2,
        .bytes = (uint8_t *)calloc(ix + 2, sizeof(uint8_t)),
    };

    s.bytes[0] = 0x02;
    s.bytes[1] = (uint8_t)ix;
    for (int i = 0; i < ix; i++) {
        s.bytes[i + 2] = buffer[ix - i - 1];
    }

    return s;
}

slice pack_null(const field *f) {
    slice s = {
        .capacity = 2,
        .length = 2,
        .bytes = (uint8_t *)calloc(2, sizeof(uint8_t)),
    };

    s.bytes[0] = 0x05;
    s.bytes[1] = 0x00;

    return s;
}

slice pack_OID(const field *f) {
    slice buffer = {
        .capacity = 64,
        .length = 0,
        .bytes = (uint8_t *)calloc(64, sizeof(uint8_t)),
    };

    char *oid = strdup(f->OID.OID);
    char *saveptr;
    char *p = strtok_r(oid, ".", &saveptr);

    // ... skip root '0'
    if (p != NULL && strcmp(p, "0") == 0) {
        p = strtok_r(NULL, ".", &saveptr);
    }

    // ... special encoding for first byte (.1.3)
    if (p != NULL) {
        uint8_t b = 0;

        uint32_t v = 0;
        if (sscanf(p, "%lu", &v) == 1) {
            b = 40 * v;
        }

        if ((p = strtok_r(NULL, ".", &saveptr)) != NULL) {
            uint32_t v = 0;
            if (sscanf(p, "%lu", &v) == 1) {
                b += v;
            }
        }

        buffer.bytes[buffer.length++] = b;

        p = strtok_r(NULL, ".", &saveptr);
    }

    // ... remaining sub-OIDs
    while (p != NULL) {
        uint32_t v = 0;
        if (sscanf(p, "%lu", &v) == 1) {
            slice bytes = pack_varint(v);

            slice_append(&buffer, bytes);
            slice_free(&bytes);
        }

        p = strtok_r(NULL, ".", &saveptr);
    }

    free(oid);

    // ... copy to slice
    slice length = pack_varint(buffer.length);

    slice s = {
        .capacity = 16 + buffer.length,
        .length = 0,
        .bytes = (uint8_t *)calloc(16 + buffer.length, sizeof(uint8_t)),
    };

    s.bytes[s.length++] = 0x06;
    slice_append(&s, length);
    slice_append(&s, buffer);

    slice_free(&length);
    slice_free(&buffer);

    return s;
}

slice pack_sequence(const field *f) {
    // ... encode fields
    slice buffer = make_slice(64);

    if (f->sequence.fields != NULL) {
        field *e;

        for (int i = 0; i < f->sequence.fields->size; i++) {
            if ((e = f->sequence.fields->fields[i]) != NULL) {
                slice v = BER_encode(*e);

                slice_append(&buffer, v);
                slice_free(&v);
            }
        }
    }

    // ... copy to slice
    slice s = make_slice(16 + buffer.length);
    slice length = pack_varint(buffer.length);

    s.bytes[s.length++] = 0x30;
    slice_append(&s, length);
    slice_append(&s, buffer);

    slice_free(&length);
    slice_free(&buffer);

    return s;
}

// Encodes a uint32 varint as a big-endian varint
slice pack_varint(const uint32_t length) {
    uint8_t buffer[8] = {0};
    uint32_t v = length;
    int ix = sizeof(buffer) - 1;

    buffer[ix--] = v & 0x7f;
    v >>= 7;

    while (v != 0) {
        buffer[ix--] = 0x80 | (v & 0x7f);
        v >>= 7;
    };

    // ... copy to slice
    slice s = {
        .capacity = 16,
        .length = 0,
        .bytes = (uint8_t *)calloc(16, sizeof(uint8_t)),
    };

    ix++;
    while (ix < sizeof(buffer)) {
        s.bytes[s.length++] = buffer[ix++];
    }

    return s;
}
