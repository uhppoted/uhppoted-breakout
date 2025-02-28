#pragma once

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
extern const uint32_t MSG_TRACE;
extern const uint32_t MSG_LOG;
extern const uint32_t MSG_WATCHDOG;

extern uart_inst_t *CLI_UART;
extern const uint CLI_IRQ;
extern const uint CLI_TX;
extern const uint CLI_RX;

extern uart_inst_t *SSMP_UART;
extern const uint SSMP_IRQ;
extern const uint SSMP_TX;
extern const uint SSMP_RX;

typedef enum {
    ERR_OK,
    ERR_MEMORY,
    ERR_I2C_GENERIC,
    ERR_I2C_TIMEOUT,
    ERR_QUEUE_FULL,
    ERR_RX8900SA,
    ERR_U2,
    ERR_U3,
    ERR_U4,
    ERR_VLF,
    ERR_WATCHDOG,
    ERR_DEBUG,
    ERR_UNKNOWN,
} err;

typedef enum {
    MESSAGE_NONE,
    MESSAGE_UINT32,
    MESSAGE_BUFFER,
    MESSAGE_SWIPE,
    MESSAGE_UNKNOWN,
} msg_type;

struct circular_buffer;

typedef struct message {
    uint32_t message;
    msg_type tag;
    union {
        uint32_t none;
        uint32_t u32;
        struct circular_buffer *buffer;
        struct swipe *swipe;
    };
} message;

bool push(message m);
