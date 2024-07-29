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
#include <cli.h>
#include <log.h>
#include <ssmp.h>
#include <sys.h>

#define _VERSION "v0.0"
#define _I2C0SDA 12
#define _I2C0SCL 13
#define _I2C1SDA 26
#define _I2C1SCL 27

const char *VERSION = _VERSION;
const uint32_t WATCHDOG_TIMEOUT = 5000; // ms
const uint32_t MSG = 0xf0000000;
const uint32_t MSG_DEBUG = 0x00000000;
const uint32_t MSG_WIO = 0x10000000;
const uint32_t MSG_U3 = 0x20000000;
const uint32_t MSG_RX = 0x30000000;
const uint32_t MSG_TTY = 0xc0000000;
const uint32_t MSG_USB = 0xd0000000;
const uint32_t MSG_WATCHDOG = 0xe0000000;
const uint32_t MSG_TICK = 0xf0000000;

queue_t queue;

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(_VERSION));
    bi_decl(bi_2pins_with_func(_I2C0SDA, _I2C0SCL, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(_I2C1SDA, _I2C1SCL, GPIO_FUNC_I2C));

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

    // ... initialise RTC, IO expanders and serial port
    RTC_init();
    IOX_init();
    ssmp_init();

    sleep_ms(1000); // FIXME remove - delay to let USB initialise

    // ... good to go, start RTC, IO expanders and serial port
    RTC_start();
    IOX_start();
    ssmp_start();

    // ... run loop
    while (true) {
        uint32_t v;
        queue_remove_blocking(&queue, &v);
        dispatch(v);

        if ((v & MSG) == MSG_USB) {
            bool connected = (v & 0x0fffffff) == 1;
            infof("SYS", "USB connected: %s", connected ? "yes" : "no");
            if (connected) {
                cli_init();
            }
        }
    }
}
