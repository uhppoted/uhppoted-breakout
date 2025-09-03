#pragma once

#include <stdbool.h>
#include <stdint.h>

extern const uint8_t NORMALLY_OPEN;
extern const uint8_t NORMALLY_CLOSED;
extern const uint8_t CONTROLLED;

bool doors_get_interlock(uint8_t *interlock);
bool doors_set_interlock(uint8_t interlock);
bool doors_interlocked(uint8_t door, uint8_t interlock);

bool doors_get_mode(uint8_t door, uint8_t *mode);
bool doors_get_delay(uint8_t door, uint8_t *delay);
bool doors_get_unlocked(uint8_t door, bool *unlocked);
bool doors_get_open(uint8_t door, bool *open);
bool doors_get_pushbutton(uint8_t door, bool *pressed);

bool doors_set_mode(uint8_t door, uint8_t mode);
bool doors_set_delay(uint8_t door, uint8_t delay);

bool doors_lock(uint8_t door);
bool doors_unlock(uint8_t door);
