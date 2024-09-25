#pragma once

#include <stdbool.h>
#include <stdint.h>

bool auth_authorised(const char *community, const char *oid);
bool auth_validate(const char *community, uint32_t code);
void auth_setkey(const char *community, const uint8_t *key, size_t length);
