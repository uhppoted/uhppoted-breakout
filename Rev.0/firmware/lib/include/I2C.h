#pragma once

#include <hardware/i2c.h>

extern int I2C_write(i2c_inst_t *bus, uint8_t addr, uint8_t reg, uint8_t data);
extern int I2C_write_all(i2c_inst_t *bus, uint8_t addr, uint8_t reg, uint8_t data[], int N);
extern int I2C_read(i2c_inst_t *bus, uint8_t addr, uint8_t reg, uint8_t *data);
extern int I2C_read_all(i2c_inst_t *bus, uint8_t addr, uint8_t reg, uint8_t *data, int N);
extern void I2C_scan(i2c_inst_t *bus, const char *title);
