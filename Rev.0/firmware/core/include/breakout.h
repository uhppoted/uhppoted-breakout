#pragma once

#include <I2C.h>
#include <pico/util/queue.h>

extern const uint32_t MSG;
extern const uint32_t MSG_DEBUG;
extern const uint32_t MSG_RX;
extern const uint32_t MSG_TTY;
extern const uint32_t MSG_WIO;
extern const uint32_t MSG_INPUTS;
extern const uint32_t MSG_TICK;

extern const uint I2C0_SDA;
extern const uint I2C0_SCL;
extern const uint I2C1_SDA;
extern const uint I2C1_SCL;

extern const uint IOX_RESET;
extern const uint IOX_INT0;
extern const uint IOX_INT1;

extern const uint SPI_RX;
extern const uint SPI_TX;
extern const uint SPI_CSn;
extern const uint SPI_CLK;

extern const uint UART0_TX;
extern const uint UART0_RX;

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
    ERR_UNKNOWN,
} err;

typedef struct buffer {
    int N;
    uint8_t *data;
} buffer;
