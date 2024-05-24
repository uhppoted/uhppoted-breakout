#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>

#include <I2C0.h>
#include <RX8900SA.h>
#include <breakout.h>
#include <sys.h>

#define VERSION "v0.0"

const uint32_t MSG = 0xf0000000;
const uint32_t MSG_RX = 0x20000000;

queue_t queue;

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(VERSION));

    stdio_init_all();
    setup_uart();

    // ... initialise FIFO, UART and timers
    queue_init(&queue, sizeof(uint32_t), 64);
    setup_uart();
    alarm_pool_init_default();

    // ... initialise RX8900SA

    I2C0_init();
    // RX8900SA_init(U5);

    // ... run loop

    printf(">> BREAKOUT %s\n", VERSION);

    while (true) {
        uint32_t v;
        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }
}
