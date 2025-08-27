#include <U3.h>
#include <U4.h>
#include <doors.h>
#include <settings.h>

#define LOGTAG "DOORS"

const uint8_t NORMALLY_OPEN = 1;
const uint8_t NORMALLY_CLOSED = 2;
const uint8_t CONTROLLED = 3;

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
