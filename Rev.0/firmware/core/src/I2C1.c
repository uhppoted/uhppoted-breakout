#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <I2C.h>
#include <I2C1.h>
#include <breakout.h>

void I2C1_init() {
    uint f = i2c_init(i2c1, 100 * 1000); // 100kHz

    gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_SDA);
    gpio_pull_up(I2C1_SCL);
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