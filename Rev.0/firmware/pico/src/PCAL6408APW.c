#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <PCAL6408APW.h>
#include <breakout.h>

const uint8_t U2 = 0x40;
const uint8_t U3 = 0x42;
const uint8_t READ = 0x01;
const uint8_t WRITE = 0x00;

const uint8_t INPUT = 0x00;
const uint8_t OUTPUT = 0x01;
const uint8_t POLARITY = 0x02;
const uint8_t CONFIGURATION = 0x03;

void initU3() {
    printf("--- INIT U3\n");

    // ... reset
    gpio_init(U3_RESET);
    gpio_set_dir(U3_RESET, GPIO_OUT);

    gpio_put(U3_RESET, 0);
    sleep_us(5);
    gpio_put(U3_RESET, 1);
    sleep_us(10);

    // // ... test input port
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

    // ... read INPUT port

    int err = i2c_write_blocking(i2c0, U3 | WRITE, &INPUT, 1, true);
    uint8_t data;

    if (err == PICO_ERROR_GENERIC) {
        printf("   ... command INPUTS err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... command INPUTS err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... command INPUTS ok:%d\n", err);
    }

    // err = i2c_read_blocking(i2c0, U3 | READ, &data, 1, true);
    // if (err == PICO_ERROR_GENERIC) {
    //     printf("   ... read    INPUTS err:%d (PICO_ERROR_GENERIC)\n", err);
    // } else if (err == PICO_ERROR_TIMEOUT) {
    //     printf("   ... read    INPUTS err:%d (PICO_ERROR_TIMEOUT)\n", err);
    // } else {
    //     printf("   ... read    INPUTS ok:%d %02x\n", err, data);
    // }
}