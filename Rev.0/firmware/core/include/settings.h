#pragma once

#include <stdint.h>

#include <doors.h>

typedef struct settings {
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

extern void settings_save();
extern void settings_restore();

extern settings SETTINGS;
