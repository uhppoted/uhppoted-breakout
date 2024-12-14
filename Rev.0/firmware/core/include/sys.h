#pragma once

#include <pico/time.h>

typedef enum {
    MODE_UNKNOWN,
    MODE_NONE,
    MODE_LOG,
    MODE_CLI,
} mode;

extern bool sys_init();
extern bool sys_on_tick(repeating_timer_t *t);
extern void sys_tick();
extern void sys_reboot();
extern void sys_watchdog_update();
extern int sys_id(char *ID, int N);

extern void syscheck();
extern void dispatch(uint32_t);

extern void print(const char *);
extern void set_mode(mode);
extern mode get_mode();

extern uint32_t get_total_heap();
extern uint32_t get_free_heap();
