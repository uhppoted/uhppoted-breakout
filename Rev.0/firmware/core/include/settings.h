#pragma once

#include <stdint.h>

#include <doors.h>

typedef enum {
    UNKNOWN,
    INTERLOCK,
    DOOR1_MODE,
    DOOR2_MODE,
    DOOR3_MODE,
    DOOR4_MODE,
    DOOR1_DELAY,
    DOOR2_DELAY,
    DOOR3_DELAY,
    DOOR4_DELAY,
} SETTING;

typedef struct setting {
    SETTING tag;
    union {
        uint8_t uint8;
    } value;
} setting;

typedef struct settings {
    uint32_t version;

    struct {
        Interlock interlock;

        struct {
            DoorMode mode;
            uint8_t delay;
        } door1;

        struct {
            DoorMode mode;
            uint8_t delay;
        } door2;

        struct {
            DoorMode mode;
            uint8_t delay;
        } door3;

        struct {
            DoorMode mode;
            uint8_t delay;
        } door4;
    } doors;
} settings;

extern setting settings_get(SETTING);
extern void settings_set(setting);
extern void settings_save();
extern void settings_restore();

extern settings SETTINGS;
