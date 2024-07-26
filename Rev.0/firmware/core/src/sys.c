#include <stdio.h>
#include <stdlib.h>

#include <hardware/watchdog.h>
#include <pico/stdlib.h>
#include <pico/sync.h>

#include <U2.h>
#include <U3.h>
#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <smp.h>
#include <sys.h>

struct {
    mode mode;
    queue_t queue;
    mutex_t guard;
} SYSTEM;

void sysinit() {
    queue_init(&SYSTEM.queue, sizeof(char *), 64);
    mutex_init(&SYSTEM.guard);
}

void set_mode(mode mode) {
    SYSTEM.mode = mode;
}

void dispatch(uint32_t v) {
    if ((v & MSG) == MSG_DEBUG) {
        debugf("SYS", "... debug??");
    }

    if ((v & MSG) == MSG_WIO) {
        U2_wio(v & 0x0000ffff);
    }

    if ((v & MSG) == MSG_U3) {
        U3_process(v & 0x000000ff);
    }

    if ((v & MSG) == MSG_RX) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
        smp_rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_TTY) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));

        if (SYSTEM.mode == MODE_SMP) {
            smp_rx(b);
        } else {
            cli_rx(b);
        }

        free(b);
    }

    if ((v & MSG) == MSG_TICK) {
        sys_tick();
    }

    if ((v & MSG) == MSG_WATCHDOG) {
        watchdog_update();
    }
}

void print(const char *msg) {
    mutex_enter_blocking(&SYSTEM.guard);
    printf("%s", msg);
    mutex_exit(&SYSTEM.guard);
}

void println(const char *msg) {
    mutex_enter_blocking(&SYSTEM.guard);
    printf("%s\n", msg);
    mutex_exit(&SYSTEM.guard);
}