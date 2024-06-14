#include <stdio.h>
#include <stdlib.h>

#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include <I2C0.h>
#include <I2C1.h>
#include <IOX.h>
#include <RTC.h>
#include <SPI.h>
#include <breakout.h>
#include <log.h>
#include <sys.h>

#define VERSION "v0.0"
#define I2C0SDA 12
#define I2C0SCL 13
#define I2C1SDA 26
#define I2C1SCL 27

const uint32_t MSG = 0xf0000000;
const uint32_t MSG_DEBUG = 0x00000000;
const uint32_t MSG_WIO = 0x10000000;
const uint32_t MSG_INPUTS = 0x20000000;
const uint32_t MSG_RX = 0x30000000;
const uint32_t MSG_SPI = 0x40000000;
const uint32_t MSG_TICK = 0xf0000000;

queue_t queue;

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(VERSION));
    bi_decl(bi_2pins_with_func(I2C0SDA, I2C0SCL, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(I2C1SDA, I2C1SCL, GPIO_FUNC_I2C));

    stdio_init_all();

    if (!sysinit()) {
        warnf("SYS", "ERROR INITIALISING SYSTEM");
        return -1;
    }

    I2C0_init();
    I2C1_init();

    multicore_launch_core1(I2C0_run);

    sleep_ms(1000);
    printf(">> BREAKOUT %s\n", VERSION);

    // ... initialise FIFO, timers and I2C
    queue_init(&queue, sizeof(uint32_t), 64);
    alarm_pool_init_default();

    // ... initialise RTC, IO expanders and SPI
    RTC_init();
    IOX_init();
    SPI_init();

    // ... run loop
    while (true) {
        uint32_t v;
        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }
}
