#include <hardware/uart.h>
#include <pico/stdlib.h>

#include <breakout.h>
#include <log.h>
#include <txrx.h>

void on_txrx();

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

void TXRX_init() {
    debugf("TXRX", "initialise");

    // ... UART
    uart_init(uart0, BAUD_RATE);

    gpio_set_function(UART0_TX, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX, GPIO_FUNC_UART);

    uart_set_baudrate(uart0, BAUD_RATE);
    uart_set_format(uart0, DATA_BITS, STOP_BITS, PARITY);
    uart_set_hw_flow(uart0, false, false);
    uart_set_fifo_enabled(uart0, false);

    irq_set_exclusive_handler(UART0_IRQ, on_txrx);
    irq_set_enabled(UART0_IRQ, true);

    uart_set_irq_enables(uart0, true, false);

    infof("TXRX", "initialised");
}

void on_txrx() {
    char buffer[32];
    int ix = 0;

    while (uart_is_readable(uart0) && ix < sizeof(buffer)) {
        buffer[ix++] = uart_getc(uart0);
    }

    if (ix > 0) {
        debugf("TXRX", ">>>>>>>>>>>>>> woooooot N:%d %02x '%c'", ix, buffer[0], buffer[0]);
        // char *b;
        //
        // if ((b = calloc(ix + 1, 1)) != NULL) {
        //     memmove(b, buffer, ix);
        //     uint32_t msg = MSG_RX | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
        //     if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        //         set_error(ERR_QUEUE_FULL, "UART", "rx: queue full");
        //         free(b);
        //     }
        // }

        ix = 0;
    }
}
