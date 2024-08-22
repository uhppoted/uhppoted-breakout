#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

void field_free(field *f) {
    if (f->tag == FIELD_SEQUENCE) {
        // fields_free(f->sequence.fields);
    } else {
        free(f);
    }
}

vector *vector_new() {
    int capacity = 16;
    size_t size = sizeof(int) + sizeof(int) + capacity * sizeof(field *);
    vector *v = (vector *)malloc(size);

    if (v != NULL) {
        v->capacity = capacity;
        v->size = 0;
        for (int i = 0; i < capacity; i++) {
            v->fields[i] = NULL;
        }
    }

    return v;
}

void vector_free(vector *v) {
    if (v != NULL) {
        free(v);
    }
}

vector *vector_add(vector *v, field *f) {
    if (v->size < v->capacity) {
        v->fields[v->size] = f;
        v->size++;

        return v;
    }

    return v;
}
