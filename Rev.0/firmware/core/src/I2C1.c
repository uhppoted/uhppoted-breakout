#include <stdio.h>
#include <string.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C.h>
#include <I2C1.h>
#include <breakout.h>

struct {
    queue_t queue;
    mutex_t guard;
} I2C1;

void I2C1_init() {
    if (strncasecmp(I2C1_CLOCK, "100kHz", 6) == 0) {
        i2c_init(i2c1, 100 * 1000); // 100kHz
    } else {
        i2c_init(i2c1, 400 * 1000); // 400kHz
    }

    gpio_init(I2C1_SCL);
    gpio_init(I2C1_SDA);

    gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(I2C1_SDA);
    gpio_pull_up(I2C1_SCL);

    // ... not strictly necessary
    gpio_set_slew_rate(I2C1_SDA, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(I2C1_SCL, GPIO_SLEW_RATE_SLOW);

    gpio_set_drive_strength(I2C0_SDA, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(I2C0_SCL, GPIO_DRIVE_STRENGTH_2MA);

    // ... init struct
    mutex_init(&I2C1.guard);
}

void I2C1_run(void (*f)()) {
    mutex_enter_blocking(&I2C1.guard);
    f();
    mutex_exit(&I2C1.guard);
}

void I2C1_scan() {
    mutex_enter_blocking(&I2C1.guard);
    I2C_scan(i2c1, "I2C1 bus scan");
    mutex_exit(&I2C1.guard);
}
