#include <stdio.h>

#include <U3.h>
#include <U4.h>
#include <doors.h>
#include <settings.h>

#define LOGTAG "DOORS"

typedef struct {
    uint8_t interlock;
    uint8_t door;
    uint8_t mask;
} interlock_t;

// clang-format off
const interlock_t interlocks[] = {
    {INTERLOCK_12,    1, 0x02},
    {INTERLOCK_12,    2, 0x01},

    {INTERLOCK_34,    3, 0x08},
    {INTERLOCK_34,    4, 0x04},

    {INTERLOCK_12_34, 1, 0x02},
    {INTERLOCK_12_34, 2, 0x01},
    {INTERLOCK_12_34, 3, 0x08},
    {INTERLOCK_12_34, 4, 0x04},

    {INTERLOCK_123,   1, 0x02 | 0x04},
    {INTERLOCK_123,   2, 0x01 | 0x04},
    {INTERLOCK_123,   3, 0x01 | 0x02},

    {INTERLOCK_1234,  1, 0x02 | 0x04 | 0x08},
    {INTERLOCK_1234,  2, 0x01 | 0x04 | 0x08},
    {INTERLOCK_1234,  3, 0x01 | 0x02 | 0x08},
    {INTERLOCK_1234,  4, 0x01 | 0x02 | 0x04},
};
// clang-format on

#define NUM_INTERLOCKS (sizeof(interlocks) / sizeof(interlocks[0]))

bool doors_get_interlock(uint8_t *interlock) {
    *interlock = SETTINGS.doors.interlock;
    return true;
}

bool doors_set_interlock(uint8_t interlock) {
    if (interlock == 0 || interlock == 1 || interlock == 2 || interlock == 3 || interlock == 4 || interlock == 8) {
        SETTINGS.doors.interlock = interlock;
        return true;
    }

    return false;
}

bool doors_interlocked(uint8_t door, uint8_t interlock) {
    // ... invalid interlock?
    if (interlock != NO_INTERLOCK && interlock != INTERLOCK_12 && interlock != INTERLOCK_34 && interlock != INTERLOCK_12_34 && interlock != INTERLOCK_123 && interlock != INTERLOCK_1234) {
        return false;
    }

    // .. no interlock?
    if (interlock == NO_INTERLOCK) {
        return false;
    }

    // ... match interlock against door unlocked/open
    uint8_t unlocked = 0x00;
    uint8_t open = 0x00;
    uint8_t mask = 0x00;

    for (int i = 1; i <= 4; i++) {
        uint8_t bit = 1u << (i - 1);
        bool b;

        // fail-safe: if door state can't be read, assume interlock is active
        if (!doors_get_unlocked(i, &b)) {
            return true;
        } else if (b) {
            unlocked |= bit;
        }

        // fail-safe: if door state can't be read, assume interlock is active
        if (!doors_get_open(i, &b)) {
            return true;
        } else if (b) {
            open |= bit;
        }
    }

    for (int i = 0; i < NUM_INTERLOCKS; i++) {
        interlock_t v = interlocks[i];

        if (v.interlock == interlock && v.door == door) {
            mask = v.mask;
            break;
        }
    }

    if ((mask & unlocked) != 0x00) {
        return true;
    }

    if ((mask & open) != 0x00) {
        return true;
    }

    return false;
}

bool doors_get_mode(uint8_t door, uint8_t *mode) {
    switch (door) {
    case 1:
        *mode = SETTINGS.doors.door1.mode;
        return true;

    case 2:
        *mode = SETTINGS.doors.door2.mode;
        return true;

    case 3:
        *mode = SETTINGS.doors.door3.mode;
        return true;

    case 4:
        *mode = SETTINGS.doors.door4.mode;
        return true;
    }

    return false;
}

bool doors_set_mode(uint8_t door, uint8_t mode) {
    if (mode == 1 || mode == 2 || mode == 3) {
        switch (door) {
        case 1:
            SETTINGS.doors.door1.mode = mode;
            return true;

        case 2:
            SETTINGS.doors.door2.mode = mode;
            return true;

        case 3:
            SETTINGS.doors.door3.mode = mode;
            return true;

        case 4:
            SETTINGS.doors.door4.mode = mode;
            return true;
        }
    }

    return false;
}

bool doors_get_delay(uint8_t door, uint8_t *delay) {
    switch (door) {
    case 1:
        *delay = SETTINGS.doors.door1.delay;
        return true;

    case 2:
        *delay = SETTINGS.doors.door2.delay;
        return true;

    case 3:
        *delay = SETTINGS.doors.door3.delay;
        return true;

    case 4:
        *delay = SETTINGS.doors.door4.delay;
        return true;
    }

    return false;
}

bool doors_set_delay(uint8_t door, uint8_t delay) {
    if (delay > 0 && delay <= 60) {
        switch (door) {
        case 1:
            SETTINGS.doors.door1.delay = delay;
            return true;

        case 2:
            SETTINGS.doors.door2.delay = delay;
            return true;

        case 3:
            SETTINGS.doors.door3.delay = delay;
            return true;

        case 4:
            SETTINGS.doors.door4.delay = delay;
            return true;
        }
    }

    return false;
}

bool doors_get_unlocked(uint8_t door, bool *unlocked) {
    switch (door) {
    case 1:
        *unlocked = U4_get_relay(1);
        return true;

    case 2:
        *unlocked = U4_get_relay(2);
        return true;

    case 3:
        *unlocked = U4_get_relay(3);
        return true;

    case 4:
        *unlocked = U4_get_relay(4);
        return true;
    }

    return false;
}

bool doors_get_open(uint8_t door, bool *open) {
    switch (door) {
    case 1:
        *open = U3_get_door(1);
        return true;

    case 2:
        *open = U3_get_door(2);
        return true;

    case 3:
        *open = U3_get_door(3);
        return true;

    case 4:
        *open = U3_get_door(4);
        return true;
    }

    return false;
}

bool doors_get_pushbutton(uint8_t door, bool *pressed) {
    switch (door) {
    case 1:
        *pressed = U3_get_button(1);
        return true;

    case 2:
        *pressed = U3_get_button(2);
        return true;

    case 3:
        *pressed = U3_get_button(3);
        return true;

    case 4:
        *pressed = U3_get_button(4);
        return true;
    }

    return false;
}

bool doors_lock(uint8_t door) {
    if (door < 1 || door > 4) {
        return false;
    }

    U4_clear_relay(door);
    return true;
}

bool doors_unlock(uint8_t door) {
    if (door < 1 || door > 4) {
        return false;
    }

    U4_set_relay(door, 5000);
    return true;
}
