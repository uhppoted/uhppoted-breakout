#include <stdio.h>

#include <I2C.h>

bool reserved_addr(uint8_t addr);

// Performs a 1-byte dummy read of all unreserved I2C addresses.
void I2C_scan(i2c_inst_t *bus, const char *title) {
    printf("\n%s\n", title);
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // ... skip over any reserved addresses.
        uint8_t data;

        if (reserved_addr(addr)) {
            printf("x");
        } else if (i2c_read_blocking(bus, addr, &data, 1, false) < 0) {
            printf(".");
        } else {
            printf("@");
        }

        printf(addr % 16 == 15 ? "\n" : "  ");
    }
}

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}
