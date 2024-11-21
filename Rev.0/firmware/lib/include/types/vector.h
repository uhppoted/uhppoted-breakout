#pragma once

#include <stdbool.h>

typedef struct vector {
    bool dynamic;
    int capacity;
    int size;
    struct field *fields[];
} vector;

extern vector *vector_new();
extern void vector_free(struct vector *);
extern vector *vector_add(struct vector *, struct field *);
