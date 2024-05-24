#include <stdio.h>
#include <string.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

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

int I2C0_write(uint8_t addr, uint8_t reg, uint8_t data) {
    debugf("I2C0", "write addr:%02x  register:%02x  data:%02x", addr, reg, data);

    uint8_t bytes[2] = {reg, data};
    int err;

    if ((err = i2c_write_blocking(i2c0, addr >> 1, bytes, 2, true)) != 2) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "write addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)", addr, reg, err);
        } else if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "write addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
        } else {
            warnf("I2C0", "write addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        }

        return err;
    }

    return err - 1;
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
        } else if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "write-all addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
        } else {
            warnf("I2C0", "write-all addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        }

        return err;
    }

    return err - 1;
}

int I2C0_read(uint8_t addr, uint8_t reg, uint8_t *data) {
    debugf("I2C0", "read");

    int err;

    if ((err = i2c_write_blocking(i2c0, addr >> 1, &reg, 1, true)) != 1) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)", addr, reg, err);
        } else if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
        } else {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        }

        return -1000;
    }

    if ((err = i2c_read_blocking(i2c0, addr >> 1, data, 1, false)) != 1) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_GENERIC)", reg, err);
        } else if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", reg, err);
        } else {
            warnf("I2C0", "read          reg:%02x  err:%d (UNKNOWN)", reg, err);
        }

        return err;
    }

    return 1;
}

int I2C0_read_all(uint8_t addr, uint8_t reg, uint8_t *data, int N) {
    debugf("I2C0", "read-all");

    int err;

    if ((err = i2c_write_blocking(i2c0, addr >> 1, &reg, 1, true)) != 1) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_GENERIC)", addr, reg, err);
        } else if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", addr, reg, err);
        } else if (err < 1) {
            warnf("I2C0", "read addr:%02x  reg:%02x  err:%d (UNKNOWN)", addr, reg, err);
        }

        return -1000;
    }

    if ((err = i2c_read_blocking(i2c0, addr >> 1, data, N, false)) != N) {
        if (err == PICO_ERROR_GENERIC) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_GENERIC)", reg, err);
        } else if (err == PICO_ERROR_TIMEOUT) {
            warnf("I2C0", "read          reg:%02x  err:%d (PICO_ERROR_TIMEOUT)", reg, err);
        } else {
            warnf("I2C0", "read          reg:%02x  err:%d (UNKNOWN)", reg, err);
        }

        return err;
    }

    return N;
}

// // I2C reserves some addresses for special purposes. We exclude these from the scan.
// // These are any addresses of the form 000 0xxx or 111 1xxx
// bool reserved_addr(uint8_t addr) {
//     return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
// }
//
// int main() {
//     stdio_init_all();
//
//     i2c_init(i2c0, 100 * 1000);
//     gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
//     gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
//     gpio_pull_up(I2C0_SDA);
//     gpio_pull_up(I2C0_SCL);
//
//     // bi_decl(bi_2pins_with_func(I2C0_SDA, I2C0_SCL, GPIO_FUNC_I2C));
//     gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
//     gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
//
//     printf("\nI2C Bus Scan\n");
//     printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
//
//     for (int addr = 0; addr < (1 << 7); ++addr) {
//         if (addr % 16 == 0) {
//             printf("%02x ", addr);
//         }
//
//         // Perform a 1-byte dummy read from the probe address. If a slave
//         // acknowledges this address, the function returns the number of bytes
//         // transferred. If the address byte is ignored, the function returns
//         // -1.
//
//         // Skip over any reserved addresses.
//         int ret;
//         uint8_t rxdata;
//         if (reserved_addr(addr))
//             ret = PICO_ERROR_GENERIC;
//         else
//             ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);
//
//         printf(ret < 0 ? "." : "@");
//         printf(addr % 16 == 15 ? "\n" : "  ");
//     }
//     printf("Done.\n");
//     return 0;
// }