#pragma once

#include <hardware/i2c.h>

typedef struct I2C {
    i2c_inst_t *bus;
    uint8_t addr;
} I2C;

extern int I2C_write(I2C dev, uint8_t reg, uint8_t data);
extern int I2C_write_all(I2C dev, uint8_t reg, uint8_t data[], int N);
extern int I2C_read(I2C dev, uint8_t reg, uint8_t *data);
extern int I2C_read_all(I2C dev, uint8_t reg, uint8_t *data, int N);
extern void I2C_scan(i2c_inst_t *bus, const char *title);

extern int I2C_write_allx(I2C dev, uint8_t reg, uint8_t data[], int N);
