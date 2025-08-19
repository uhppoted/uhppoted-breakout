#pragma once

#include <I2C.h>
#include <hardware/uart.h>
#include <pico/util/queue.h>

extern const uint32_t MSG;
extern const uint32_t MSG_DEBUG;
extern const uint32_t MSG_RX;
extern const uint32_t MSG_TTY;
extern const uint32_t MSG_WIO;
extern const uint32_t MSG_SWIPE;
extern const uint32_t MSG_KEYCODE;
extern const uint32_t MSG_U3;
extern const uint32_t MSG_TICK;
extern const uint32_t MSG_EVENT;
extern const uint32_t MSG_SAVE;
extern const uint32_t MSG_TRACE;
extern const uint32_t MSG_LOG;
extern const uint32_t MSG_WATCHDOG;

extern const uint I2C0_SDA;
extern const uint I2C0_SCL;
extern const uint I2C1_SDA;
extern const uint I2C1_SCL;

extern const uint IOX_RESET;
extern const uint IOX_INT0;
extern const uint IOX_INT1;

extern const uint UART0_TX;
extern const uint UART0_RX;

extern const uint UART1_TX;
extern const uint UART1_RX;

extern const struct I2C U2;
extern const struct I2C U3;
extern const struct I2C U4;
extern const struct I2C U5;

typedef enum {
    ERR_OK,
    ERR_RESTART,
    ERR_MEMORY,
    ERR_I2C,
    ERR_QUEUE,
    ERR_RX8900SA,
    ERR_RX8900SA_VLF, // internal error for RX8900SA use only
    ERR_U2,
    ERR_U3,
    ERR_U4,
    ERR_WATCHDOG,
    ERR_DEBUG,
    ERR_UNKNOWN,
} err;

typedef enum {
    EVENT_UNKNOWN,

    EVENT_SYS_START,
    EVENT_SYS_RESET,

    EVENT_DOOR_1_OPEN,
    EVENT_DOOR_2_OPEN,
    EVENT_DOOR_3_OPEN,
    EVENT_DOOR_4_OPEN,

    EVENT_DOOR_1_CLOSE,
    EVENT_DOOR_2_CLOSE,
    EVENT_DOOR_3_CLOSE,
    EVENT_DOOR_4_CLOSE,

    EVENT_DOOR_1_PRESSED,
    EVENT_DOOR_2_PRESSED,
    EVENT_DOOR_3_PRESSED,
    EVENT_DOOR_4_PRESSED,

    EVENT_DOOR_1_RELEASED,
    EVENT_DOOR_2_RELEASED,
    EVENT_DOOR_3_RELEASED,
    EVENT_DOOR_4_RELEASED,

    EVENT_DOOR_1_LOCKED,
    EVENT_DOOR_2_LOCKED,
    EVENT_DOOR_3_LOCKED,
    EVENT_DOOR_4_LOCKED,

    EVENT_DOOR_1_UNLOCKED,
    EVENT_DOOR_2_UNLOCKED,
    EVENT_DOOR_3_UNLOCKED,
    EVENT_DOOR_4_UNLOCKED,

    EVENT_DOOR_1_SWIPE,
    EVENT_DOOR_2_SWIPE,
    EVENT_DOOR_3_SWIPE,
    EVENT_DOOR_4_SWIPE,
} EVENT;

typedef enum {
    MESSAGE_NONE,
    MESSAGE_UINT32,
    MESSAGE_BUFFER,
    MESSAGE_SWIPE,
    MESSAGE_EVENT,
    MESSAGE_UNKNOWN,
} msg_type;

struct circular_buffer;

typedef struct message {
    uint32_t message;
    msg_type tag;
    union {
        uint32_t none;
        uint32_t u32;
        uint32_t event;
        struct circular_buffer *buffer;
        struct swipe *swipe;
    };
} message;

bool push(message m);
void syserr_set(err e, const char *tag, const char *fmt, ...);
bool syserr_get(err e);
void syserr_clear(err e);
uint16_t syserr_bitmask();