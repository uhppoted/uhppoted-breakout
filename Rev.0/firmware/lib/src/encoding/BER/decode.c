#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

extern void fields_free(fields);
extern fields fields_add(fields, field *);

fields unpack(const uint8_t *message, int N);
void unpack_boolean(const uint8_t *message, int N, int *ix);
void unpack_null(const uint8_t *message, int N, int *ix);
void unpack_OID(const uint8_t *message, int N, int *ix);
field *unpack_sequence(const uint8_t *message, int N, int *ix);

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
            // case 1:
            //     unpack_boolean(bytes, N, &ix);
            //     break;

            // case 5:
            //     unpack_null(bytes, N, &ix);
            //     break;

            // case 6:
            //     unpack_OID(bytes, N, &ix);
            //     break;

        case 48:
            if ((f = unpack_sequence(bytes, N, &ix)) != NULL) {
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

void unpack_boolean(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 0;
    if (*ix < N) {
        uint8_t b = message[*ix];
        *ix += 1;

        if (b & 0x80 == 0x80) {
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

    printf("::boolean     N:%d  ix:%-3d length:%lu  end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;
}

void unpack_null(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 0;
    if (*ix < N) {
        uint8_t b = message[*ix];
        *ix += 1;

        if (b & 0x80 == 0x80) {
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

    printf("::null        N:%d  ix:%-3d length:%lu  end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;
}

void unpack_OID(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 0;
    if (*ix < N) {
        uint8_t b = message[*ix];
        *ix += 1;

        if (b & 0x80 == 0x80) {
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

    printf("::OID         N:%d  ix:%-3d length:%lu  end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;
}

field *unpack_sequence(const uint8_t *message, int N, int *ix) {
    // ... length
    uint32_t length = 0;
    if (*ix < N) {
        uint8_t b = message[*ix];
        *ix += 1;

        if (b & 0x80 == 0x80) {
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

    printf("::sequence    N:%d  ix:%-3d length:%lu end:%d\n", N, *ix, length, *ix + (int)length);

    *ix += length;

    field *f = (field *)calloc(1, sizeof(field));

    return f;
}
