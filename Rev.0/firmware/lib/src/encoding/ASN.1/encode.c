#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <types/slice.h>

slice pack_integer(const field *f);
slice pack_octets(const field *f);
slice pack_null(const field *f);
slice pack_OID(const field *f);
slice pack_sequence(const field *f);
slice pack_pdu(const field *f);
slice pack_varint(const uint32_t length);

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

    case FIELD_OCTET_STRING:
        s = pack_octets(&f);
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

    case FIELD_PDU_GET_RESPONSE:
        s = pack_pdu(&f);
        break;
    }

    return s;
}

slice pack_integer(const field *f) {
    uint8_t buffer[9] = {0};
    int ix = 0;
    int64_t v = f->integer.value;
    uint8_t u8 = 0x00;

    do {
        u8 = (uint8_t)(v & 0x00ff);
        buffer[ix++] = u8;
        v >>= 8;
    } while (v != 0 && v != -1 && ix < sizeof(buffer));

    // ... pad if +ve and most-significant bit is set OR -ve and most-significant bit is not set
    if ((f->integer.value > 0) && (u8 > 0x007f)) {
        buffer[ix++] = 0x00;
    } else if ((f->integer.value < 0) && (u8 < 0x80)) {
        buffer[ix++] = 0xff;
    }

    slice s = {
        .capacity = 64,
        .length = 0,
        .bytes = (uint8_t *)calloc(64, sizeof(uint8_t)),
    };

    s.bytes[s.length++] = 0x02;

    slice length = pack_varint(ix);

    slice_append(&s, length); // NTS: should be reversed but only ever 1 byte
    slice_free(&length);

    for (int i = 0; i < ix; i++) {
        s.bytes[s.length++] = buffer[ix - i - 1];
    }

    return s;
}

slice pack_octets(const field *f) {
    slice s = {
        .capacity = 64,
        .length = 0,
        .bytes = (uint8_t *)calloc(64, sizeof(uint8_t)),
    };

    s.bytes[s.length++] = 0x04;

    slice length = pack_varint(f->octets.length);
    slice octets = {
        .capacity = f->octets.length,
        .length = f->octets.length,
        .bytes = (uint8_t *)calloc(f->octets.length, sizeof(uint8_t)),
    };

    memmove(octets.bytes, f->octets.octets, f->octets.length);

    slice_append(&s, length);
    slice_append(&s, octets);

    slice_free(&length);
    slice_free(&octets);

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
    slice buffer = slice_make(64);

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
    slice s = slice_make(16 + buffer.length);
    slice length = pack_varint(buffer.length);

    s.bytes[s.length++] = 0x30;
    slice_append(&s, length);
    slice_append(&s, buffer);

    slice_free(&length);
    slice_free(&buffer);

    return s;
}

slice pack_pdu(const field *f) {
    // ... encode fields
    slice buffer = slice_make(64);

    if (f->pdu.fields != NULL) {
        field *e;

        for (int i = 0; i < f->pdu.fields->size; i++) {
            if ((e = f->pdu.fields->fields[i]) != NULL) {
                slice v = BER_encode(*e);

                slice_append(&buffer, v);
                slice_free(&v);
            }
        }
    }

    // ... copy to slice
    slice s = slice_make(16 + buffer.length);
    slice length = pack_varint(buffer.length);

    switch (f->tag) {
    case FIELD_PDU_GET:
        s.bytes[s.length++] = 0xA0;
        break;

    case FIELD_PDU_GET_RESPONSE:
        s.bytes[s.length++] = 0xA2;
        break;
    }

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
