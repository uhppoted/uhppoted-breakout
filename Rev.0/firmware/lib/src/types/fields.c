#include <log.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/ASN.1/BER.h>
#include <types/fields.h>

const int CAPACITY = 16;

void field_free(field *f) {
    if (f != NULL) {
        switch (f->tag) {
        case FIELD_OCTET_STRING:
            free(f->octets.octets);
            break;

        case FIELD_OID:
            free(f->OID.OID);
            break;

        case FIELD_SEQUENCE:
            vector_free(f->sequence.fields);
            break;

        case FIELD_PDU_GET:
            // vector_free(f->pdu.fields);
            // free(f->pdu.fields);
            break;
        }

        if (f->dynamic) {
            free(f);
        }
    }
}

vector *vector_new() {
    int capacity = CAPACITY;
    size_t size = sizeof(vector) + capacity * sizeof(struct field *);
    vector *v = (vector *)calloc(sizeof(uint8_t), size);

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
    if (v != NULL) {
        for (int i = 0; i < v->size; i++) {
            field_free(v->fields[i]);
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
    vector *u = (vector *)calloc(sizeof(uint8_t), size);

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

        if (v->dynamic) {
            free(v);
        }

        return vector_add(u, f);
    }

    return v;
}
