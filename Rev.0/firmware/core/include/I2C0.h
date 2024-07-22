#pragma once

struct closure;

typedef void (*F)(void *);

typedef struct closure {
    F f;
    void *data;
} closure;

extern void I2C0_init();
extern void I2C0_scan();
extern void I2C0_run();
extern bool I2C0_push(const struct closure *);
