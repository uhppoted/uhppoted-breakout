#include <stdio.h>
#include <stdlib.h>

#include <pico/stdlib.h>

#include <hardware/watchdog.h>

#include <U2.h>
#include <U3.h>
#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <smp.h>
#include <sys.h>

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
        SMP_rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_TTY) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));
        cli_rx(b);
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
    printf("%s", msg);
}

void println(const char *msg) {
    printf("%s\n", msg);
}