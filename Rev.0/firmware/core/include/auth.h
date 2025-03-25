#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    OP_UNKNOWN,
    OP_GET,
    OP_SET,
} OP;

bool auth_authorised(const char *community, const char *oid, OP op);
bool auth_validate(const char *community, uint32_t code);
void auth_setkey(const char *community, const uint8_t *key, size_t length);
