#include <stdio.h>
#include <string.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <I2C.h>
#include <breakout.h>
#include <log.h>

void I2C0_init() {
    uint f = i2c_init(i2c0, 100 * 1000); // 100kHz

    infof("I2C0", "init %lukHz", f / 1000);

    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA);
    gpio_pull_up(I2C0_SCL);
}

void I2C0_scan() {
    I2C_scan(i2c0, "I2C0 bus scan");
}

int I2C0_write(uint8_t addr, uint8_t reg, uint8_t data) {
    debugf("I2C0", "write addr:%02x  register:%02x  data:%02x", addr, reg, data);

    uint8_t bytes[2] = {reg, data};
    int err;

    if ((err = i2c_write_blocking(i2c0, addr >> 1, bytes, 2, true)) != 2) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "write addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)", addr, reg, err);
            return ERR_GENERIC;
        }

        if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "write addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
            return ERR_TIMEOUT;
        }

        warnf("I2C0", "write addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        return ERR_UNKNOWN;
    }

    return 0;
}

int I2C0_write_all(uint8_t addr, uint8_t reg, uint8_t data[], int N) {
    debugf("I2C0", "write-all addr:%02x  register:%02x  N:%d", addr, reg, N);

    uint8_t bytes[N + 1];
    int err;

    bytes[0] = reg;
    memmove(&bytes[1], data, N);

    if ((err = i2c_write_blocking(i2c0, addr >> 1, bytes, N + 1, true)) != N + 1) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "write-all addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)", addr, reg, err);
            return ERR_GENERIC;
        }

        if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "write-all addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
            return ERR_TIMEOUT;
        }

        warnf("I2C0", "write-all addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        return ERR_UNKNOWN;
    }

    return 0;
}

int I2C0_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    debugf("I2C0", "read");

    int err;

    if ((err = i2c_write_blocking(i2c0, addr >> 1, &reg, 1, true)) != 1) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)", addr, reg, err);
            return ERR_GENERIC;
        }

        if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
            return ERR_TIMEOUT;
        }

        warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        return ERR_UNKNOWN;
    }

    if ((err = i2c_read_blocking(i2c0, addr >> 1, data, 1, false)) != 1) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_GENERIC)", reg, err);
            return ERR_GENERIC;
        }

        if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", reg, err);
            return ERR_TIMEOUT;
        }

        warnf("I2C0", "read          reg:%02x  err:%d (UNKNOWN)", reg, err);
        return ERR_UNKNOWN;
    }

    return 0;
}

int I2C0_read_all(uint8_t addr, uint8_t reg, uint8_t *data, int N) {
    debugf("I2C0", "read-all");

    int err;

    if ((err = i2c_write_blocking(i2c0, addr >> 1, &reg, 1, true)) != 1) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)", addr, reg, err);
            return ERR_GENERIC;
        }

        if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
            return ERR_TIMEOUT;
        }

        warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        return ERR_UNKNOWN;
    }

    if ((err = i2c_read_blocking(i2c0, addr >> 1, data, N, false)) != N) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_GENERIC)", reg, err);
            return ERR_GENERIC;
        }

        if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", reg, err);
            return ERR_TIMEOUT;
        }

        warnf("I2C0", "read          reg:%02x  err:%d (UNKNOWN)", reg, err);
        return ERR_UNKNOWN;
    }

    return 0;
}
