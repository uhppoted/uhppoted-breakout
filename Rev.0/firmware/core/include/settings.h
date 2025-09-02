#pragma once

#include <stdint.h>

typedef struct settings {
    struct {
        uint8_t interlock;

        struct {
            uint8_t mode;
            uint8_t delay;
        } door1;

        struct {
            uint8_t mode;
            uint8_t delay;
        } door2;

        struct {
            uint8_t mode;
            uint8_t delay;
        } door3;

        struct {
            uint8_t mode;
            uint8_t delay;
        } door4;
    } doors;
} settings;

extern void settings_save();
extern void settings_restore();

extern settings SETTINGS;
