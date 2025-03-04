#include <stddef.h>

#include <types/buffer.h>

bool buffer_push(circular_buffer *buffer, uint8_t ch) {
    int head = buffer->head;
    int next = (head + 1) % sizeof(buffer->bytes);

    if (next == buffer->tail) {
        return false;
    } else {
        buffer->bytes[head] = ch;
        buffer->head = next;
    }

    return true;
}

bool buffer_pop(circular_buffer *buffer, uint8_t *ch) {
    int tail = buffer->tail;

    if (tail == buffer->head) {
        return false;
    }

    *ch = buffer->bytes[tail++];
    buffer->tail = tail % sizeof(buffer->bytes);

    return true;
}

bool buffer_empty(circular_buffer *buffer) {
    return buffer->tail == buffer->head;
}

void buffer_flush(circular_buffer *buffer, void (*f)(uint8_t)) {
    int tail = buffer->tail;
    uint8_t ch;

    while (tail != buffer->head) {
        ch = buffer->bytes[tail++];
        tail %= sizeof(buffer->bytes);

        if (f != NULL) {
            f(ch);
        }
    }

    buffer->tail = tail;
}
