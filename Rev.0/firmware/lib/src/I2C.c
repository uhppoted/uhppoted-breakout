#include <stdio.h>
#include <string.h>

#include <I2C.h>
#include <breakout.h>
#include <sys.h>

bool reserved_addr(uint8_t addr);

// Writes a single byte to an I2C device.
int I2C_write(I2C dev, uint8_t reg, uint8_t data) {
    uint8_t bytes[2] = {reg, data};
    int err;

    if ((err = i2c_write_blocking(dev.bus, dev.addr >> 1, bytes, 2, true)) != 2) {
        if ((err == PICO_ERROR_GENERIC) || (err == PICO_ERROR_TIMEOUT)) {
            return ERR_I2C;
        }

        return ERR_UNKNOWN;
    }

    return 0;
}

// Writes a block of bytes to an I2C address + register.
int I2C_write_all(I2C dev, uint8_t reg, uint8_t data[], int N) {
    uint8_t bytes[N + 1];
    int err;

    bytes[0] = reg;
    memmove(&bytes[1], data, N);

    if ((err = i2c_write_blocking(dev.bus, dev.addr >> 1, bytes, N + 1, true)) != N + 1) {
        if ((err == PICO_ERROR_GENERIC) || (err == PICO_ERROR_TIMEOUT)) {
            return ERR_I2C;
        }

        return ERR_UNKNOWN;
    }

    return 0;
}

// Reads a single byte from an address + register.
int I2C_read(I2C dev, uint8_t reg, uint8_t *data) {
    int err;

    if ((err = i2c_write_blocking(dev.bus, dev.addr >> 1, &reg, 1, true)) != 1) {
        if ((err == PICO_ERROR_GENERIC) || (err == PICO_ERROR_TIMEOUT)) {
            return ERR_I2C;
        }

        return ERR_UNKNOWN;
    }

    if ((err = i2c_read_blocking(dev.bus, dev.addr >> 1, data, 1, false)) != 1) {
        if ((err == PICO_ERROR_GENERIC) || (err == PICO_ERROR_TIMEOUT)) {
            return ERR_I2C;
        }

        return ERR_UNKNOWN;
    }

    return 0;
}

// Reads a contiguous block of N bytes into the data array.
int I2C_read_all(I2C dev, uint8_t reg, uint8_t *data, int N) {
    int err;

    if ((err = i2c_write_blocking(dev.bus, dev.addr >> 1, &reg, 1, true)) != 1) {
        if ((err == PICO_ERROR_GENERIC) || (err == PICO_ERROR_TIMEOUT)) {
            printf(">>> AWOOGAH/1\n");
            return ERR_I2C;
        }

        printf(">>> AWOOGAH/2\n");
        return ERR_UNKNOWN;
    }

    if ((err = i2c_read_blocking(dev.bus, dev.addr >> 1, data, N, false)) != N) {
        if ((err == PICO_ERROR_GENERIC) || (err == PICO_ERROR_TIMEOUT)) {
            printf(">>> AWOOGAH/3\n");
            return ERR_I2C;
        }

        printf(">>> AWOOGAH/4\n");
        return ERR_UNKNOWN;
    }

    return 0;
}

// Performs a 1-byte dummy read of all unreserved I2C addresses.
void I2C_scan(i2c_inst_t *bus, const char *title) {
    char s[128];
    int ix = 0;

    snprintf(s, sizeof(s), "\n%s\n", title);
    snprintf(s, sizeof(s), "   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            ix += sprintf(&s[ix], "%02x ", addr);
        }

        // ... skip over any reserved addresses.
        uint8_t data;

        if (reserved_addr(addr)) {
            ix += sprintf(&s[ix], "x");
        } else if (i2c_read_blocking(bus, addr, &data, 1, false) < 0) {
            ix += sprintf(&s[ix], ".");
        } else {
            ix += sprintf(&s[ix], "@");
        }

        if (addr % 16 == 15) {
            sprintf(&s[ix], "\n");
            print(s);
            ix = 0;
            continue;
        }

        ix += sprintf(&s[ix], "  ");
    }
}

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}
