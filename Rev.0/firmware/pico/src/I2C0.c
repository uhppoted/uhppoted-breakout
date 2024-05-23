#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <breakout.h>

const uint8_t READ = 0x01;
const uint8_t WRITE = 0x00;

void I2C0_init() {
    printf("--- I2C0::init\n");

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
}

int I2C0_write(uint8_t addr, uint8_t reg, uint8_t data) {
    printf("--- I2C0::write\n");

    int err = i2c_write_blocking(i2c0, (addr >> 1) | WRITE, &reg, 1, true);

    if (err == PICO_ERROR_GENERIC) {
        printf("   ... write addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)\n", addr, reg, err);
        return err;
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... write addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)\n", addr, reg, err);
        return err;
    } else if (err < 1) {
        printf("   ... write addr:%02x  reg:%02x  err:%d (UNKNOWN)\n", addr, reg, err);
    } else {
        printf("   ... write addr:%02x  reg:%02x  ok:%d\n", addr, reg, err);

        err = i2c_write_blocking(i2c0, (addr >> 1) | WRITE, &data, 1, false);
        if (err == PICO_ERROR_GENERIC) {
            printf("   ... write  reg:%02x  err:%d (PICO_ERROR_GENERIC)\n", reg, err);
            return err;
        } else if (err == PICO_ERROR_TIMEOUT) {
            printf("   ... write          reg:%02x  err:%d (PICO_ERROR_TIMEOUT)\n", reg, err);
            return err;
        } else if (err < 1) {
            printf("   ... write          reg:%02x  err:%d (UNKNOWN)\n", reg, err);
        } else {
            printf("   ... write          reg:%02x  ok:%d %02x\n", reg, err, data);
        }
    }
}

int I2C0_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    printf("--- I2C0::write\n");

    int err = i2c_write_blocking(i2c0, (addr >> 1) | WRITE, &reg, 1, true);

    if (err == PICO_ERROR_GENERIC) {
        printf("   ... read addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)\n", addr, reg, err);
        return err;
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... read addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)\n", addr, reg, err);
        return err;
    } else if (err < 1) {
        printf("   ... read addr:%02x  reg:%02x  err:%d (UNKNOWN)\n", addr, reg, err);
    } else {
        printf("   ... read addr:%02x  reg:%02x  ok:%d\n", addr, reg, err);

        err = i2c_read_blocking(i2c0, (addr >> 1) | WRITE, data, 1, false);
        if (err == PICO_ERROR_GENERIC) {
            printf("   ... read          reg:%02x  err:%d (PICO_ERROR_GENERIC)\n", reg, err);
            return err;
        } else if (err == PICO_ERROR_TIMEOUT) {
            printf("   ... read          reg:%02x  err:%d (PICO_ERROR_TIMEOUT)\n", reg, err);
            return err;
        } else {
            printf("   ... read          reg:%02x  ok:%d %02x\n", reg, err, *data);
        }
    }
}