#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <encoding/encoding.h>

struct message BER_encode(const struct packet p);
struct packet BER_decode(const uint8_t *message, int N);
