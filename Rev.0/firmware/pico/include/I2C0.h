#pragma once

extern void I2C0_init();
extern int I2C0_write(uint8_t addr,uint8_t reg, uint8_t data);
extern int I2C0_write_all(uint8_t addr,uint8_t reg, uint8_t data[], int N);
extern int I2C0_read(uint8_t addr,uint8_t reg, uint8_t *data);
extern int I2C0_read_all(uint8_t addr,uint8_t reg, uint8_t *data, int N);