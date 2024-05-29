#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <I2C.h>
#include <I2C0.h>
#include <breakout.h>

void I2C0_init() {
    uint f = i2c_init(i2c0, 400 * 1000); // 100kHz

    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA);
    gpio_pull_up(I2C0_SCL);
}

int I2C0_write(uint8_t addr, uint8_t reg, uint8_t data) {
    return I2C_write(i2c0, addr, reg, data);
}

int I2C0_write_all(uint8_t addr, uint8_t reg, uint8_t data[], int N) {
    return I2C_write_all(i2c0, addr, reg, data, N);
}

int I2C0_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    return I2C_read(i2c0, addr, reg, data);
}

int I2C0_read_all(uint8_t addr, uint8_t reg, uint8_t *data, int N) {
    return I2C_read_all(i2c0, addr, reg, data, N);
}

void I2C0_scan() {
    I2C_scan(i2c0, "I2C0 bus scan");
}
