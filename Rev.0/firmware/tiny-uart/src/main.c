#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include <hardware/watchdog.h>

#include <I2C0.h>
#include <I2C1.h>
#include <IOX.h>
#include <RTC.h>
#include <SSMP.h>
#include <breakout.h>
#include <log.h>
#include <mempool.h>
#include <sys.h>

#define LOGTAG "SYS"
#define _VERSION "v0.0"
#define _I2C0SDA 12
#define _I2C0SCL 13
#define _I2C1SDA 26
#define _I2C1SCL 27

queue_t queue;

const uint32_t WATCHDOG_TIMEOUT = 5000; // ms

int main() {
    bi_decl(bi_program_description("uhppoted-breakout"));
    bi_decl(bi_program_version_string(_VERSION));
    bi_decl(bi_2pins_with_func(_I2C0SDA, _I2C0SCL, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(_I2C1SDA, _I2C1SCL, GPIO_FUNC_I2C));

    stdio_init_all();

    queue_init(&queue, sizeof(uint32_t), 64);
    alarm_pool_init_default();

    if (!mempool_init()) {
        warnf("SYS", "*** ERROR INITIALISING MEMPOOL");
        return -1;
    }

    // ... initialise system
    if (!sys_init()) {
        warnf("SYS", "*** ERROR INITIALISING SYSTEM");
        return -1;
    }

    syserr_set(ERR_RESTART, LOGTAG, "power-on");

    if (watchdog_caused_reboot()) {
        syserr_set(ERR_WATCHDOG, LOGTAG, "watchdog reboot");
    } 

    if (strcmp(WATCHDOG, "disabled") != 0) {
        watchdog_enable(WATCHDOG_TIMEOUT, true);
    }

    // ... initialise FIFO, timers and I2C
    I2C0_init();
    I2C1_init();

    multicore_launch_core1(I2C0_run);

    // ... initialise RTC, IO expanders and serial port
    RTC_init();
    IOX_init();
    SSMP_init();

    sleep_ms(2500); // FIXME remove - delay to let USB initialise

    // ... good to go, start RTC, IO expanders and serial port
    RTC_start();
    IOX_start();
    SSMP_start();

    // ... POR events
    push((message){
        .message = MSG_EVENT,
        .tag = MESSAGE_EVENT,
        .event = EVENT_SYS_START,
    });

    if (watchdog_caused_reboot()) {
        push((message){
            .message = MSG_EVENT,
            .tag = MESSAGE_EVENT,
            .event = EVENT_SYS_RESET,
        });
    }

    // ... run loop
    while (true) {
        uint32_t v;

        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }

    return 0;
}
