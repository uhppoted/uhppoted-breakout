#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>

void vector_free(vector *);

const int CAPACITY = 16;

void field_free(field *const f) {
    switch (f->tag) {
    case FIELD_OCTET_STRING:
        free(f->octets.octets);
        break;

    case FIELD_OID:
        free(f->OID.OID);
        break;

    case FIELD_SEQUENCE:
        vector_free(f->sequence.fields);
        free(f->sequence.fields);
        break;

    case FIELD_PDU_GET:
        vector_free(f->pdu.fields);
        free(f->pdu.fields);
        break;
    }
}

vector *vector_new() {
    int capacity = CAPACITY;
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

void vector_free(vector *const v) {
    if (v != NULL) {
        for (int i = 0; i < v->size; i++) {
            field_free(v->fields[i]);
        }
    }
}

vector *vector_add(vector *v, field *f) {
    if (v->size < v->capacity) {
        v->fields[v->size] = f;
        v->size++;

        return v;
    }

    // .. reallocate and copy
    int capacity = v->capacity + CAPACITY;
    size_t size = sizeof(int) + sizeof(int) + capacity * sizeof(field *);
    vector *u = (vector *)malloc(size);

    if (u != NULL) {
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
