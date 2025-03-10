#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <log.h>
#include <types/buffer.h>

#define LOGTAG "UART"
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

void on_uart1_rx();

struct {
    circular_buffer buffer;
} SERIAL = {
    .buffer = {
        .head = 0,
        .tail = 0,
    },
};

bool uart1_init() {
    gpio_pull_up(UART1_TX);
    gpio_pull_up(UART1_RX);

    gpio_set_function(UART1_TX, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX, GPIO_FUNC_UART);

    uart_init(uart1, BAUD_RATE);
    uart_set_baudrate(uart1, BAUD_RATE);
    uart_set_format(uart1, DATA_BITS, STOP_BITS, PARITY);
    uart_set_hw_flow(uart1, false, false);
    uart_set_fifo_enabled(uart1, true);
    uart_set_translate_crlf(uart1, false);

    irq_set_exclusive_handler(UART1_IRQ, on_uart1_rx);
    uart_set_irq_enables(uart1, true, false);
    irq_set_enabled(UART1_IRQ, true);

    infof(LOGTAG, "initialised");

    return true;
}

void on_uart1_rx() {
    while (uart_is_readable(uart1)) {
        uint8_t ch = uart_getc(uart1);

        buffer_push(&SERIAL.buffer, ch);
    }

    message qmsg = {
        .message = MSG_RX,
        .tag = MESSAGE_BUFFER,
        .buffer = &SERIAL.buffer,
    };

    push(qmsg);
}

void uart1_write(const uint8_t *bytes, int N) {
    uart_write_blocking(uart1, bytes, N);
}
