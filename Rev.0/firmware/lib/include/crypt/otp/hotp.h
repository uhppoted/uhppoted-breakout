#pragma once

#include <stdbool.h>
#include <stdint.h>

bool hotp_validate(const uint8_t *secret, int length, uint64_t counter, uint32_t code);
