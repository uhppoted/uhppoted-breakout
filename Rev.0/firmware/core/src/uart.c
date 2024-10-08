#include <stdlib.h>
#include <string.h>

#include <hardware/uart.h>

#include <breakout.h>
#include <state.h>
#include <uart.h>

void on_uart_rx() {
    char buffer[32];
    int ix = 0;

    while (uart_is_readable(uart0) && ix < sizeof(buffer)) {
        buffer[ix++] = uart_getc(uart0);
    }

    if (ix > 0) {
        struct buffer *b;

        if ((b = (struct buffer *)malloc(sizeof(struct buffer))) != NULL) {
            b->N = ix;
            memmove(b->data, buffer, ix);

            uint32_t msg = MSG_TTY | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
            if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                set_error(ERR_QUEUE_FULL, "TTY", "rx: queue full");
                free(b);
            }
        }

        ix = 0;
    }
}
