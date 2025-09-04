#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    NORMALLY_OPEN = 1,
    NORMALLY_CLOSED = 2,
    CONTROLLED = 3,
} DoorMode;

typedef enum {
    NO_INTERLOCK = 0,    // disabled
    INTERLOCK_12 = 1,    // doors (1,2)
    INTERLOCK_34 = 2,    // doors (3,4)
    INTERLOCK_12_34 = 3, // doors (1,2) and (3,4)
    INTERLOCK_123 = 4,   // doors (1,2,3)
    INTERLOCK_1234 = 8,  // doors (1,2,3,4)
} Interlock;

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
