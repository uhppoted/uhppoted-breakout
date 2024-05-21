#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/stdlib.h>

#include <PCAL6408APW.h>
#include <breakout.h>

const uint8_t U2 = 0x40;
const uint8_t U3 = 0x42;
const uint8_t READ = 0x01;
const uint8_t WRITE = 0x00;
const uint8_t MASK = 0xfe;

const uint8_t INPUTS = 0x00;
const uint8_t OUTPUTS = 0x01;
const uint8_t POLARITY = 0x02;
const uint8_t CONFIGURATION = 0x03;

void initU3() {
    printf("--- INIT U3\n");

    i2c_init(i2c0, 100 * 1000); // I2C0, 100kHz
    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);

    // ... read input port
    // int err = i2c_write_timeout_us(i2c0, U3, &INPUTS, 1, true, 1000);
    int err = i2c_write_blocking(i2c0, U3, &INPUTS, 1, true);

    if (err == PICO_ERROR_GENERIC) {
        printf("   ... set register to INPUTS err:%d (PICO_ERROR_GENERIC)\n", err);
    } else if (err == PICO_ERROR_TIMEOUT) {
        printf("   ... set register to INPUTS err:%d (PICO_ERROR_TIMEOUT)\n", err);
    } else {
        printf("   ... set register to INPUTS ok:%d\n", err);
    }
}