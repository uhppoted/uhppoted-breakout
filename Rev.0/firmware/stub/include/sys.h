#pragma once

#include <pico/time.h>

extern bool sys_init();
extern void sys_tick();
extern void sys_trace();
extern void sys_reboot();
extern int sys_id(char *ID, int N);

extern void syscheck();
extern void dispatch(uint32_t);

extern void print(const char *);
extern void set_trace(float interval);

extern uint32_t get_total_heap();
extern uint32_t get_free_heap();
