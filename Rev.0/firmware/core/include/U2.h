#pragma once

#include <stdint.h>

extern void U2_setup();
extern void U2_start();
extern void U2_wio(uint8_t inputs);
extern void U2_on_card_read(uint32_t v);
