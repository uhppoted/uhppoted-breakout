#include <stdio.h>
#include <stdlib.h>

#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include <hardware/watchdog.h>

#include <I2C0.h>
#include <I2C1.h>
#include <IOX.h>
#include <RTC.h>
#include <breakout.h>
#include <log.h>
#include <smp.h>
#include <sys.h>

#define VERSION "v0.0"
#define I2C0SDA 8
#define I2C0SCL 9
#define I2C1SDA 2
#define I2C1SCL 3

const uint32_t MSG = 0xf0000000;
const uint32_t MSG_DEBUG = 0x00000000;
const uint32_t MSG_WIO = 0x10000000;
const uint32_t MSG_U3 = 0x20000000;
const uint32_t MSG_RX = 0x30000000;
const uint32_t MSG_TTY = 0xd0000000;
const uint32_t MSG_WATCHDOG = 0xe0000000;
const uint32_t MSG_TICK = 0xf0000000;

const uint32_t WATCHDOG_TIMEOUT = 5000; // ms

queue_t queue;

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(VERSION));
    bi_decl(bi_2pins_with_func(I2C0SDA, I2C0SCL, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(I2C1SDA, I2C1SCL, GPIO_FUNC_I2C));

    stdio_init_all();
    watchdog_enable(WATCHDOG_TIMEOUT, true);
    queue_init(&queue, sizeof(uint32_t), 64);
    alarm_pool_init_default();

    if (!sys_init()) {
        warnf("SYS", "ERROR INITIALISING SYSTEM");
        return -1;
    }

    // ... initialise FIFO, timers and I2C
    I2C0_init();
    I2C1_init();

    multicore_launch_core1(I2C0_run);

    printf(">> BREAKOUT %s\n", VERSION);

    // ... initialise RTC, IO expanders and serial port
    RTC_init();
    IOX_init();
    SMP_init();

    // ... good to go, start RTC, IO expanders and serial port
    RTC_start();
    IOX_start();
    SMP_start();

    // ... run loop
    while (true) {
        uint32_t v;
        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }
}
