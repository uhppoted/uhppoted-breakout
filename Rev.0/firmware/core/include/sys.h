#pragma once

extern bool sys_init();
extern void sys_tick();
extern void sys_reboot();
extern void dispatch(uint32_t);

extern void print(const char *);
extern void println(const char *);
