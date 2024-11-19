#pragma once

typedef enum {
    MODE_UNKNOWN,
    MODE_CLI,
} mode;

extern bool sys_init();
extern void sys_tick();
extern void sys_reboot();
extern void sys_watchdog_update();
extern int sys_id(char *ID, int N);
extern void dispatch(uint32_t);

extern void print(const char *);
extern void println(const char *);
extern void set_mode(mode);
extern mode get_mode();
