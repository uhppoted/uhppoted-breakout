#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hardware/watchdog.h>
#include <pico/stdlib.h>
#include <pico/sync.h>

#include <U2.h>
#include <U3.h>
#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <ssmp.h>
#include <sys.h>

struct {
    mode mode;
    queue_t queue;
    mutex_t guard;
} SYSTEM;

const int32_t FLUSH = 1000; // ms

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
        ssmp_rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_TTY) {
        char *b = (char *)(SRAM_BASE | (v & 0x0fffffff));

        if (SYSTEM.mode == MODE_SMP) {
            ssmp_rx(b);
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
    if (SYSTEM.mode == MODE_CLI) {
        mutex_enter_blocking(&SYSTEM.guard);
        printf("%s", msg);
        mutex_exit(&SYSTEM.guard);
    }
}

void println(const char *msg) {
    if (SYSTEM.mode == MODE_CLI) {
        mutex_enter_blocking(&SYSTEM.guard);
        printf("%s\n", msg);
        mutex_exit(&SYSTEM.guard);
    }
}

void printx(char *msg) {
    if (SYSTEM.mode != MODE_CLI || msg == NULL) {
        free(msg);
        return;
    }

    if (mutex_try_enter(&SYSTEM.guard, NULL)) {
        printf("%s", msg);
        free(msg);

        // ... write any pending messages
        char *pending = NULL;

        while (queue_try_remove(&SYSTEM.queue, &pending)) {
            int remaining = strlen(pending);
            int ix = 0;
            int N;

            while (remaining > 0) {
                fflush(stdout);
                if ((N = fwrite(&pending[ix], 1, remaining, stdout)) > 0) {
                    remaining -= N;
                    ix += N;
                } else {
                    sleep_ms(100);
                }
            }

            free(pending);
        }

        mutex_exit(&SYSTEM.guard);
        return;
    }

    if (!queue_try_add(&SYSTEM.queue, &msg)) {
        free(msg);
    }
}
