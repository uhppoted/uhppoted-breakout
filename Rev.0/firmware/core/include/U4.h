#pragma once

#include <stdbool.h>

extern void U4_init();
extern void U4_set_relay(int relay, uint16_t delay);
extern void U4_clear_relay(int relay);
extern void U4_set_LED(int LED);
extern void U4_clear_LED(int LED);
extern void U4_toggle_LED(int LED);
extern void U4_blink_LED(int LED, int count, uint16_t interval);

extern void U4_set_ERR();
extern void U4_clear_ERR();
extern void U4_blink_ERR(int count, uint16_t interval);

extern void U4_set_IN();
extern void U4_clear_IN();
extern void U4_blink_IN(int count, uint16_t interval);

extern void U4_set_SYS();
extern void U4_clear_SYS();
extern void U4_blink_SYS(int count, uint16_t interval);
