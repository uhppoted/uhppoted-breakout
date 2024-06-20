#pragma once

#include <stdbool.h>

extern void U4_init();
extern void U4_set_relay(int relay, uint16_t delay);
extern void U4_clear_relay(int relay);
extern void U4_set_LED(int LED);
extern void U4_clear_LED(int LED);
extern void U4_toggle_LED(int LED);
extern void U4_blink_LED(int LED, int count, uint16_t interval);
extern void U4_set_ERR(bool state);
extern void U4_set_IN(bool state);
extern void U4_set_SYS(bool state);
