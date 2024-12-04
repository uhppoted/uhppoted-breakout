#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <types/fields.h>

struct slice BER_encode(const struct field f);
struct vector *BER_decode(const uint8_t *message, int N);
