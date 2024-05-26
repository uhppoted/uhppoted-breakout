#include <stdio.h>

#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>

#include <I2C0.h>
#include <I2C1.h>
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
    // bi_decl(bi_2pins_with_func(I2C0_SDA, I2C0_SCL, GPIO_FUNC_I2C));
    // bi_decl(bi_2pins_with_func(I2C1_SDA, I2C1_SCL, GPIO_FUNC_I2C));

    stdio_init_all();
    setup_uart();

    printf(">> BREAKOUT %s\n", VERSION);

    // ... initialise FIFO, UART, timers and I2C
    queue_init(&queue, sizeof(uint32_t), 64);
    setup_uart();
    alarm_pool_init_default();

    I2C0_init();
    I2C1_init();

    // ... initialise RTC
    RX8900SA_init(U5);

    // ... initialise IO expanders
    gpio_init(IOX_RESET);
    gpio_set_dir(IOX_RESET, GPIO_OUT);

    gpio_put(IOX_RESET, 0);
    sleep_us(5);
    gpio_put(IOX_RESET, 1);
    sleep_us(10);

    // ... run loop
    while (true) {
        uint32_t v;
        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }
}
