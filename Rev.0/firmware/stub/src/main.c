#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include <hardware/watchdog.h>

#include <log.h>
#include <stub.h>
#include <sys.h>

#define LOGTAG "SYS"
#define _VERSION "v0.0"

queue_t queue;

const uint32_t WATCHDOG_TIMEOUT = 5000; // ms
const uint32_t MSG_TICK = 0xf0000000;

int main() {
    bi_decl(bi_program_description("uhppoted-stub"));
    bi_decl(bi_program_version_string(_VERSION));

    stdio_init_all();

    if (watchdog_caused_reboot()) {
        warnf(LOGTAG, "watchdog reboot");

        // FIXME remove - debugging
        printf(">>>> WATCHDOG REBOOT\n");
        while (true) {
            sleep_ms(2500);
            printf("     ... watchdogged\n");
        }
        // END FIXME
    } else {
        watchdog_enable(WATCHDOG_TIMEOUT, true);
    }

    queue_init(&queue, sizeof(uint32_t), 64);
    alarm_pool_init_default();

    // ... initialise system
    if (!sys_init()) {
        warnf("SYS", "*** ERROR INITIALISING SYSTEM");
        return -1;
    }

    // ... initialise RTC, IO expanders and serial port
    sleep_ms(2500); // FIXME remove - delay to let USB initialise

    // ... run loop
    while (true) {
        uint32_t v;

        queue_remove_blocking(&queue, &v);
        dispatch(v);
    }
}
