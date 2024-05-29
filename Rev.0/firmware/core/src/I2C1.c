#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <I2C.h>
#include <I2C1.h>
#include <breakout.h>

void I2C1_init() {
    uint f = i2c_init(i2c1, 400 * 1000); // 100kHz

    gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_SDA);
    gpio_pull_up(I2C1_SCL);
}

int I2C1_write(uint8_t addr, uint8_t reg, uint8_t data) {
    return I2C_write(i2c1, addr, reg, data);
}

int I2C1_write_all(uint8_t addr, uint8_t reg, uint8_t data[], int N) {
    return I2C_write_all(i2c1, addr, reg, data, N);
}

int I2C1_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    return I2C_read(i2c1, addr, reg, data);
}

int I2C1_read_all(uint8_t addr, uint8_t reg, uint8_t *data, int N) {
    return I2C_read_all(i2c1, addr, reg, data, N);
}

void I2C1_scan() {
    I2C_scan(i2c1, "I2C1 bus scan");
}

void I2C1_debug() {
    uint pin = I2C1_SCL;

    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);

    while (true) {
        printf("I2C1::OFF\n");
        gpio_put(pin, 0);
        sleep_ms(2500);
        printf("I2C1::ON\n");
        gpio_put(pin, 1);
        sleep_ms(2500);
    }
}