#include <I2C.h>
#include <pico/types.h>

// error codes
const int ERR_OK = 0;       // OK
const int ERR_GENERIC = -1; // PICO_ERROR_GENERIC
const int ERR_TIMEOUT = -2; // PICO_ERROR_TIMEOUT
const int ERR_UNKNOWN = -3;

// GPIO
const uint GPIO_0 = 0;   // Pico 1
const uint GPIO_1 = 1;   // Pico 2
const uint GPIO_2 = 2;   // Pico 4
const uint GPIO_3 = 3;   // Pico 5
const uint GPIO_4 = 4;   // Pico 6
const uint GPIO_5 = 5;   // Pico 7
const uint GPIO_6 = 6;   // Pico 9
const uint GPIO_7 = 7;   // Pico 10
const uint GPIO_8 = 8;   // Pico 11
const uint GPIO_9 = 9;   // Pico 12
const uint GPIO_10 = 10; // Pico 14
const uint GPIO_11 = 11; // Pico 15
const uint GPIO_12 = 12; // Pico 16
const uint GPIO_13 = 13; // Pico 17
const uint GPIO_14 = 14; // Pico 19
const uint GPIO_15 = 15; // Pico 20
const uint GPIO_16 = 16; // Pico 21
const uint GPIO_17 = 17; // Pico 22
const uint GPIO_18 = 18; // Pico 24
const uint GPIO_19 = 19; // Pico 25
const uint GPIO_20 = 20; // Pico 26
const uint GPIO_21 = 21; // Pico 27
const uint GPIO_22 = 22; // Pico 29
const uint GPIO_26 = 26; // Pico 31
const uint GPIO_27 = 27; // Pico 32
const uint GPIO_28 = 28; // Pico 34

const uint I2C0_SDA = GPIO_12;
const uint I2C0_SCL = GPIO_13;
const uint I2C1_SDA = GPIO_26;
const uint I2C1_SCL = GPIO_27;

const uint IOX_RESET = GPIO_14;
const uint IOX_INT0 = GPIO_15;
const uint IOX_INT1 = GPIO_9;

// U2 PCAL6408A
const struct I2C U2 = {
    .bus = i2c1,
    .addr = 0x40,
};

// U3 PCAL6408A
const struct I2C U3 = {
    .bus = i2c0,
    .addr = 0x42,
};

// U4 PI4IOE5V6416
const struct I2C U4 = {
    .bus = i2c0,
    .addr = 0x40,
};

// U5 RX8900SA
const struct I2C U5 = {
    .bus = i2c0,
    .addr = 0x64,
};