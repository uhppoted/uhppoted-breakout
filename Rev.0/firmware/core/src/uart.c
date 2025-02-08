#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>

#include <breakout.h>
#include <state.h>
#include <types/buffer.h>

struct {
    circular_buffer buffer;
} SERIAL = {
    .buffer = {
        .head = 0,
        .tail = 0,
    },
};

// NTS: **NOT TESTED** (Pico only)
void on_uart_rx() {
    while (uart_is_readable(uart0)) {
        uint8_t ch = uart_getc(uart0);

        buffer_push(&SERIAL.buffer, ch);
    }

    message qmsg = {
        .message = MSG_TTY,
        .tag = MESSAGE_BUFFER,
        .buffer = &SERIAL.buffer,
    };

    push(qmsg);
}
