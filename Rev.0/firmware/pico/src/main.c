#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>

#include <I2C0.h>
#include <I2C1.h>
#include <IOX.h>
#include <RTC.h>
#include <breakout.h>
#include <sys.h>

#define VERSION "v0.0"

const uint32_t MSG = 0xf0000000;
const uint32_t MSG_WIO = 0x10000000;
const uint32_t MSG_INPUTS = 0x20000000;
const uint32_t MSG_RX = 0x30000000;

queue_t queue;

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(VERSION));
    // bi_decl(bi_2pins_with_func(I2C0_SDA, I2C0_SCL, GPIO_FUNC_I2C));
    // bi_decl(bi_2pins_with_func(I2C1_SDA, I2C1_SCL, GPIO_FUNC_I2C));

    stdio_init_all();
    setup_uart();
    I2C0_init();
    I2C1_init();

    printf(">> BREAKOUT %s\n", VERSION);

    // ... initialise FIFO, UART, timers and I2C
    queue_init(&queue, sizeof(uint32_t), 64);
    setup_uart();
    alarm_pool_init_default();

    // ... initialise RTC and IO expanders
    RTC_init();
    IOX_init();

    // ... run loop
    while (true) {
        uint32_t v;
        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }
}