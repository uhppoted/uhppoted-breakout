#pragma once

#include <stdbool.h>

extern void U4_init();
extern void U4_set_relay(int relay, uint16_t delay);
extern void U4_clear_relay(int relay);
extern void U4_set_LED(int LED, bool state);
extern void U4_set_ERR(bool state);
extern void U4_set_IN(bool state);
extern void U4_set_SYS(bool state);
