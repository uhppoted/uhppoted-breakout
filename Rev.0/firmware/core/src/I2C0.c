#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <I2C.h>
#include <I2C0.h>
#include <breakout.h>

struct {
    queue_t queue;
} I2C0;

void I2C0_init() {
    uint f = i2c_init(i2c0, 100 * 1000); // 100kHz

    gpio_init(I2C0_SCL);
    gpio_init(I2C0_SDA);

    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(I2C0_SDA);
    gpio_pull_up(I2C0_SCL);

    queue_init(&I2C0.queue, sizeof(F), 32);
}

void I2C0_run() {
    F f;

    while (true) {
        queue_remove_blocking(&I2C0.queue, &f);
        f();
    }
}

bool I2C0_push(F f) {
    if (queue_is_full(&I2C0.queue) || !queue_try_add(&I2C0.queue, &f)) {
        printf("I2C0 .. discarded task %p\n", f);
        return false;
    }

    return true;
}

int I2C0_write(uint8_t addr, uint8_t reg, uint8_t data) {
    I2C dev = {
        .bus = i2c0,
        .addr = addr,
    };

    return I2C_write(dev, reg, data);
}

int I2C0_write_all(uint8_t addr, uint8_t reg, uint8_t data[], int N) {
    I2C dev = {
        .bus = i2c0,
        .addr = addr,
    };

    return I2C_write_all(dev, reg, data, N);
}

int I2C0_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    I2C dev = {
        .bus = i2c0,
        .addr = addr,
    };

    return I2C_read(dev, reg, data);
}

int I2C0_read_all(uint8_t addr, uint8_t reg, uint8_t *data, int N) {
    I2C dev = {
        .bus = i2c0,
        .addr = addr,
    };

    return I2C_read_all(dev, reg, data, N);
}

void I2C0_scan() {
    I2C_scan(i2c0, "I2C0 bus scan");
}
