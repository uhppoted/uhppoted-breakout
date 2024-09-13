#pragma once

typedef struct vector {
    int capacity;
    int size;
    struct field *fields[4];
} vector;
