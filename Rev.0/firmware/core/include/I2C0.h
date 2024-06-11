#pragma once

typedef struct closure {
    const void (*f)(void *);
    void *data;
} closure;

extern void I2C0_init();
extern void I2C0_scan();
extern void I2C0_debug();

extern int I2C0_write(uint8_t addr, uint8_t reg, uint8_t data);
extern int I2C0_write_all(uint8_t addr, uint8_t reg, uint8_t data[], int N);
extern int I2C0_read(uint8_t addr, uint8_t reg, uint8_t *data);
extern int I2C0_read_all(uint8_t addr, uint8_t reg, uint8_t *data, int N);

extern void I2C0_run();
extern bool I2C0_push(const struct closure *);
