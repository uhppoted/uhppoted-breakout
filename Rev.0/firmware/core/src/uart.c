#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>

#include <breakout.h>

void on_uart0_rx() {
    char buffer[32];
    int ix = 0;

    while (uart_is_readable(uart0) && ix < sizeof(buffer)) {
        buffer[ix++] = uart_getc(uart0);
    }

    if (ix > 0) {
        char *b;

        if ((b = calloc(ix + 1, 1)) != NULL) {
            memmove(b, buffer, ix);
            uint32_t msg = MSG_RX | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
            if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                free(b);
            }
        }

        ix = 0;
    }
}