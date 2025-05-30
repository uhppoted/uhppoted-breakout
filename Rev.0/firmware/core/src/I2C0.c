#include <stdio.h>
#include <string.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C.h>
#include <I2C0.h>
#include <breakout.h>

#define LOGTAG "I2C0"

struct {
    queue_t queue;
    mutex_t guard;
} I2C0;

void I2C0_init() {
    if (strncasecmp(I2C0_CLOCK, "400kHz", 6) == 0) {
        uint f = i2c_init(i2c0, 400 * 1000); // 400kHz
    } else {
        uint f = i2c_init(i2c0, 100 * 1000); // 100kHz
    }

    gpio_init(I2C0_SCL);
    gpio_init(I2C0_SDA);

    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(I2C0_SDA);
    gpio_pull_up(I2C0_SCL);

    // ... not strictly necessary
    gpio_set_slew_rate(I2C0_SDA, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(I2C0_SCL, GPIO_SLEW_RATE_SLOW);

    gpio_set_drive_strength(I2C0_SDA, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(I2C0_SCL, GPIO_DRIVE_STRENGTH_2MA);

    // ... init struct
    queue_init(&I2C0.queue, sizeof(closure), 32);
    mutex_init(&I2C0.guard);
}

void I2C0_run() {
    struct closure v;

    while (true) {
        queue_remove_blocking(&I2C0.queue, &v);

        mutex_enter_blocking(&I2C0.guard);
        v.f(v.data);
        mutex_exit(&I2C0.guard);
    }
}

bool I2C0_push(const closure *v) {
    if (queue_is_full(&I2C0.queue) || !queue_try_add(&I2C0.queue, v)) {
        syserr_set(ERR_QUEUE, LOGTAG, "push: queue full");
        return false;
    }

    return true;
}

void I2C0_scan() {
    mutex_enter_blocking(&I2C0.guard);
    I2C_scan(i2c0, "I2C0 bus scan");
    mutex_exit(&I2C0.guard);
}
