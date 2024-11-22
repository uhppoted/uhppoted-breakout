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

// NTS: Pico only - not tested
void on_uart_rx() {
    int next = (SERIAL.buffer.head + 1) % sizeof(SERIAL.buffer.bytes);
    int poke = 0;

    while (uart_is_readable(uart0)) {
        uint8_t ch = uart_getc(uart0);

        if (next != SERIAL.buffer.tail) {
            SERIAL.buffer.bytes[SERIAL.buffer.head] = ch;
            SERIAL.buffer.head = next;

            next = (SERIAL.buffer.head + 1) % sizeof(SERIAL.buffer.bytes);
        }
    }

    circular_buffer *b = &SERIAL.buffer;
    uint32_t msg = MSG_TTY | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000

    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SSMP", "rx: queue full");
    }
}
