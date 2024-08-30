#include <stdlib.h>

#include <types/slice.h>

slice make_slice(int size) {
    slice v = {
        .capacity = size,
        .length = 0,
        .bytes = (uint8_t *)calloc(size, sizeof(uint8_t)),
    };

    return v;
}

void slice_free(slice *const slice) {
    free(slice->bytes);

    slice->capacity = 0;
    slice->length = 0;
    slice->bytes = NULL;
}

void slice_append(slice *const dest, slice src) {
    for (int i = 0; i < src.length; i++) {
        dest->bytes[dest->length++] = src.bytes[i];
    }
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
