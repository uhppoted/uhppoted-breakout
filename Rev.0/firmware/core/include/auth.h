#pragma once

#include <stdbool.h>

bool authorised(const char *community, const char *oid);
bool validate(const char *community, uint32_t code);
