#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <encoding/BER/BER.h>

void field_free(field *f) {
    free(f);
}

void fields_free(fields list) {
    fieldp *p = list;

    while (*p != NULL) {
        field_free(*p);
        p++;
    }
}

fields fields_add(fields list, field *f) {
    if (f == NULL) {
        return list;
    }

    // ... count valid entries
    int N = 0;
    fieldp *p = list;

    while (*p != NULL) {
        N++;
        p++;
    }

    // ... copy 'old' list to 'new' list
    fields old = list;

    if ((list = calloc(N + 2, sizeof(fieldp))) == NULL) {
        return old;
    }

    // ... memmove maybe?
    int ix = 0;
    for (; ix < N; ix++) {
        list[ix] = old[ix];
    }

    free(old);

    // ... append new field and terminating NULL
    list[ix++] = f;
    list[ix] = NULL;

    return list;
}