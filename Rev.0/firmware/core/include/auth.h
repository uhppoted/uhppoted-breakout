#pragma once

#include <stdbool.h>

bool auth_authorised(const char *community, const char *oid);
bool auth_validate(const char *community, uint32_t code);
