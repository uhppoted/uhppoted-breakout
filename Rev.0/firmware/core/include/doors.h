#pragma once

#include <stdbool.h>
#include <stdint.h>

bool doors_get_mode(uint8_t door, uint8_t *mode);
bool doors_get_delay(uint8_t door, uint8_t *delay);
bool doors_get_unlocked(uint8_t door, bool *unlocked);
bool doors_get_open(uint8_t door, bool *open);
bool doors_get_pushbutton(uint8_t door, bool *pressed);

bool doors_unlock(uint8_t door);
