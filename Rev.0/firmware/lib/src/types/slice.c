#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <types/slice.h>

slice slice_make(int size) {
    slice v = {
        .capacity = size,
        .length = 0,
        .dynamic = false,
        .bytes = (uint8_t *)calloc(size, sizeof(uint8_t)),
    };

    return v;
}

void slice_free(slice *slice) {
    if (slice != NULL) {
        free(slice->bytes);

        slice->capacity = 0;
        slice->length = 0;
        slice->bytes = NULL;

        if (slice->dynamic) {
            free(slice);
        }
    }
}

int slice_append(slice *dest, slice src) {
    while (dest->capacity < dest->length + src.length) {
        int N = dest->capacity + 64;
        uint8_t *_old = dest->bytes;
        uint8_t *_new = (uint8_t *)calloc(N, sizeof(uint8_t));

        if (_new == NULL) {
            break;
        } else {
            memmove(_new, _old, dest->length);

            dest->bytes = _new;
            dest->capacity = N;

            free(_old);
        }
    }

    if (dest->capacity >= dest->length + src.length) {
        memmove(&dest->bytes[dest->length], src.bytes, src.length);
        dest->length += src.length;
        return src.length;
    }

    return -1;
}

// uint8_t *reverse(uint8_t *buffer, int N) {
//     uint8_t *p = &buffer[0];
//     uint8_t *q = &buffer[N - 1];
//
//     for (int i = 0; i < N / 2; i++) {
//         uint8_t v = *p;
//
//         *p++ = *q;
//         *q-- = v;
//     }
//
//     return buffer;
// }
