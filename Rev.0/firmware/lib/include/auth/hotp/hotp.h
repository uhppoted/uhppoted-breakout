#pragma once

#include <stdbool.h>
#include <stdint.h>

bool hotp_validate(const char *community, uint32_t code);
