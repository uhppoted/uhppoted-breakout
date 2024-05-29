#pragma once

#include <I2C.h>
#include <pico/util/queue.h>

extern const uint32_t MSG;
extern const uint32_t MSG_RX;
extern const uint32_t MSG_WIO;
extern const uint32_t MSG_INPUTS;

extern const uint I2C0_SDA;
extern const uint I2C0_SCL;
extern const uint I2C1_SDA;
extern const uint I2C1_SCL;

extern const uint IOX_RESET;
extern const uint IOX_INT0;
extern const uint IOX_INT1;

extern const struct I2C U2;
extern const struct I2C U3;
extern const struct I2C U5;

extern queue_t queue;

extern const int ERR_OK;
extern const int ERR_GENERIC;
extern const int ERR_TIMEOUT;
extern const int ERR_UNKNOWN;