#include <log.h>
#include <stdlib.h>
#include <string.h>

#include <types/vector.h>

const int CAPACITY = 16;

struct field;

extern void field_free(struct field *const);

vector *vector_new() {
    int capacity = CAPACITY;
    size_t size = sizeof(vector) + capacity * sizeof(struct field *);
    vector *v = (vector *)malloc(size);

    if (v != NULL) {
        v->dynamic = true;
        v->capacity = capacity;
        v->size = 0;

        for (int i = 0; i < capacity; i++) {
            v->fields[i] = NULL;
        }
    }

    return v;
}

void vector_free(vector *v) {
    struct field *f;

    if (v != NULL) {
        for (int i = 0; i < v->size; i++) {
            if ((f = v->fields[i]) != NULL) {
                field_free(v->fields[i]);
            }
        }

        if (v->dynamic) {
            free(v);
        }
    }
}

vector *vector_add(vector *v, struct field *f) {
    if (v->size < v->capacity) {
        v->fields[v->size++] = f;

        return v;
    }

    // .. reallocate and copy
    int capacity = v->capacity + CAPACITY;
    size_t size = sizeof(vector) + capacity * sizeof(struct field *);
    vector *u = (vector *)malloc(size);

    if (u != NULL) {
        u->dynamic = true;
        u->capacity = capacity;
        u->size = v->size;

        for (int i = 0; i < capacity; i++) {
            u->fields[i] = NULL;
        }

        for (int i = 0; i < v->size; i++) {
            u->fields[i] = v->fields[i];
        }

        return vector_add(u, f);
    }

    return v;
}
