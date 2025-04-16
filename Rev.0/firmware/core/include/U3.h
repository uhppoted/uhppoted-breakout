#pragma once

#include <stdbool.h>
#include <stdint.h>

extern void U3_init();
extern void U3_start();
extern void U3_process(uint8_t inputs);
extern bool U3_get_door(uint8_t door);
extern bool U3_get_button(uint8_t door);
