#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <sys.h>

#define VERSION "v0.0"

const uint32_t MSG = 0xf0000000;
const uint32_t MSG_RX = 0x20000000;

queue_t queue;

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(VERSION));

    stdio_init_all();
    setup_uart();

    // ... initialise FIFO, UART and timers
    queue_init(&queue, sizeof(uint32_t), 64);
    setup_uart();
    alarm_pool_init_default();

    // ... run loop

    printf(">> BREAKOUT %s\n", VERSION);

    while (true) {
        uint32_t v;
        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }
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