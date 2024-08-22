#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

void vector_free(vector *);

void field_free(field *f) {
    switch (f->tag) {
    case FIELD_OCTET_STRING:
        free(f->string.octets);
        break;

    case FIELD_OID:
        free(f->OID.OID);
        break;

    case FIELD_SEQUENCE:
        vector_free(f->sequence.fields);
        break;

    case FIELD_PDU:
        vector_free(f->PDU.fields);
        break;
    }

    free(f);
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
        for (int i = 0; i < v->size; i++) {
            field_free(v->fields[i]);
        }

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
