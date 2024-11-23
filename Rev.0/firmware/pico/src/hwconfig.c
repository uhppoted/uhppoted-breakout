#include <I2C.h>
#include <hardware/uart.h>
#include <pico/types.h>

// GPIO
const uint GPIO_0 = 0;
const uint GPIO_1 = 1;
const uint GPIO_2 = 2;
const uint GPIO_3 = 3;
const uint GPIO_4 = 4;
const uint GPIO_5 = 5;
const uint GPIO_6 = 6;
const uint GPIO_7 = 7;
const uint GPIO_8 = 8;
const uint GPIO_9 = 9;
const uint GPIO_10 = 10;
const uint GPIO_11 = 11;
const uint GPIO_12 = 12;
const uint GPIO_13 = 13;
const uint GPIO_14 = 14;
const uint GPIO_15 = 15;
const uint GPIO_16 = 16;
const uint GPIO_17 = 17;
const uint GPIO_18 = 18;
const uint GPIO_19 = 19;
const uint GPIO_20 = 20;
const uint GPIO_21 = 21;
const uint GPIO_22 = 22;
const uint GPIO_26 = 26;
const uint GPIO_27 = 27;
const uint GPIO_28 = 28;

const uint I2C0_SDA = GPIO_20;
const uint I2C0_SCL = GPIO_21;
const uint I2C1_SDA = GPIO_26;
const uint I2C1_SCL = GPIO_27;

const uint IOX_RESET = GPIO_16;
const uint IOX_INT1 = GPIO_19;
const uint IOX_INT0 = GPIO_22;

const uart_inst_t *CLI_UART = uart0;
const uint CLI_IRQ = UART0_IRQ;
const uint CLI_TX = GPIO_0;
const uint CLI_RX = GPIO_1;

const uart_inst_t *SSMP_UART = uart1;
const uint SSMP_IRQ = UART1_IRQ;
const uint SSMP_TX = GPIO_4;
const uint SSMP_RX = GPIO_5;

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

// U4 PCAL6416A
const struct I2C U4 = {
    .bus = i2c0,
    .addr = 0x40,
};

// U5 RX8900SA
const struct I2C U5 = {
    .bus = i2c0,
    .addr = 0x64,
};
