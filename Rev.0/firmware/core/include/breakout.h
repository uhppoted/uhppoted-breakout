#pragma once

#include <I2C.h>
#include <hardware/uart.h>
#include <pico/util/queue.h>

extern const uint32_t MSG;
extern const uint32_t MSG_DEBUG;
extern const uint32_t MSG_RX;
extern const uint32_t MSG_TTY;
extern const uint32_t MSG_WIO;
extern const uint32_t MSG_U3;
extern const uint32_t MSG_TICK;
extern const uint32_t MSG_WATCHDOG;

extern const uint I2C0_SDA;
extern const uint I2C0_SCL;
extern const uint I2C1_SDA;
extern const uint I2C1_SCL;

extern const uint IOX_RESET;
extern const uint IOX_INT0;
extern const uint IOX_INT1;

extern uart_inst_t *CLI_UART;
extern const uint CLI_IRQ;
extern const uint CLI_TX;
extern const uint CLI_RX;

extern uart_inst_t *SSMP_UART;
extern const uint SSMP_IRQ;
extern const uint SSMP_TX;
extern const uint SSMP_RX;

extern const struct I2C U2;
extern const struct I2C U3;
extern const struct I2C U4;
extern const struct I2C U5;

extern queue_t queue;

typedef enum {
    ERR_OK,
    ERR_I2C_GENERIC,
    ERR_I2C_TIMEOUT,
    ERR_QUEUE_FULL,
    ERR_RX8900SA,
    ERR_U3,
    ERR_U4,
    ERR_VLF,
    ERR_WATCHDOG,
    ERR_STDOUT,
    ERR_UNKNOWN,
} err;
