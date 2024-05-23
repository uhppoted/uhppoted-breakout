#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <PCAL6408APW.h>
#include <breakout.h>

const uint8_t U5 = 0x64;
const uint8_t U5_READ = 0x01;
const uint8_t U5_WRITE = 0x00;

const uint8_t EXTENSION = 0x0d;
const uint8_t FLAG = 0x0d;
const uint8_t CONTROL = 0x0f;

void initU5() {
    printf("--- INIT U5\n");

    // // ... test SCL/SDA
    // uint pin = I2C0_SDA;
    //
    // gpio_init(pin);
    // gpio_set_dir(pin, GPIO_OUT);
    //
    // while (true) {
    //     printf("OFF\n");
    //     gpio_put(pin, 0);
    //     sleep_ms(2500);
    //     printf("ON\n");
    //     gpio_put(pin, 1);
    //     sleep_ms(2500);
    // }

    // ... setup I2C0
    i2c_init(i2c0, 100 * 1000); // 100kHz

    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA);
    gpio_pull_up(I2C0_SCL);

    // ... reset RX8900SA
    printf("... reset RX8900SA\n");

    sleep_ms(1000);

    int err = i2c_write_blocking(i2c0, (U5 >> 1) | U5_WRITE, &EXTENSION, 1, true);
    uint8_t data = 0x08;

    if (err == PICO_ERROR_GENERIC) {
        printf("   ... command EXTENSION err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... command EXTENSION err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... command EXTENSION ok:%d\n", err);
    }

    err = i2c_write_blocking(i2c0, (U5 >> 1) | U5_WRITE, &data, 1, false);
    if (err == PICO_ERROR_GENERIC) {
        printf("   ... write   EXTENSION err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... write   EXTENSION err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... write   EXTENSION ok:%d %02x\n", err, data);
    }

    err = i2c_write_blocking(i2c0, (U5 >> 1) | U5_WRITE, &FLAG, 1, true);
    data = 0x00;

    if (err == PICO_ERROR_GENERIC) {
        printf("   ... command FLAG err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... command FLAG err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... command FLAG ok:%d\n", err);
    }

    err = i2c_write_blocking(i2c0, (U5 >> 1) | U5_WRITE, &data, 1, false);
    if (err == PICO_ERROR_GENERIC) {
        printf("   ... write   FLAG err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... write   FLAG err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... write   FLAG ok:%d %02x\n", err, data);
    }

    err = i2c_write_blocking(i2c0, (U5 >> 1) | U5_WRITE, &CONTROL, 1, true);
    data = 0x00;

    if (err == PICO_ERROR_GENERIC) {
        printf("   ... command CONTROL err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... command CONTROL err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... command CONTROL ok:%d\n", err);
    }

    err = i2c_write_blocking(i2c0, (U5 >> 1) | U5_WRITE, &data, 1, false);
    if (err == PICO_ERROR_GENERIC) {
        printf("   ... write   CONTROL err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... write   CONTROL err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... write   CONTROL ok:%d %02x\n", err, data);
    }

    // err = i2c_read_blocking(i2c0, U5 | U5_READ, &data, 1, false);
    // if (err == PICO_ERROR_GENERIC) {
    //     printf("   ... read    FLAG err:%d (PICO_ERROR_GENERIC)\n", err);
    // } else if (err == PICO_ERROR_TIMEOUT) {
    //     printf("   ... read    FLAG err:%d (PICO_ERROR_TIMEOUT)\n", err);
    // } else {
    //     printf("   ... read    FLAG ok:%d %02x\n", err, data);
    // }
}