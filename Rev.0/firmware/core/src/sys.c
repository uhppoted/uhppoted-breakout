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
} SYSTEM = {
    .mode = MODE_UNKNOWN,
};

const int32_t FLUSH = 1000; // ms

void _push(char *);
void _flush();
void _print(const char *);

void sysinit() {
    queue_init(&SYSTEM.queue, sizeof(char *), 64);
    mutex_init(&SYSTEM.guard);
}

mode get_mode() {
    switch (SYSTEM.mode) {
    case MODE_CLI:
        return MODE_CLI;

    case MODE_SSMP:
        return MODE_SSMP;

    default:
        return MODE_UNKNOWN;
    }
}

void set_mode(mode mode) {
    SYSTEM.mode = mode;

    // ... unblock queue
    if (mode == MODE_CLI) {
        print("");
    }
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
        struct buffer *b = (struct buffer *)(SRAM_BASE | (v & 0x0fffffff));
        ssmp_rx(b);
        free(b);
    }

    if ((v & MSG) == MSG_TTY) {
        struct buffer *b = (struct buffer *)(SRAM_BASE | (v & 0x0fffffff));

        if (SYSTEM.mode == MODE_SSMP) {
            ssmp_rx(b);
        } else {
            cli_rx(b);
        }

        free(b);
    }

    if ((v & MSG) == MSG_TICK) {
        sys_tick();

        switch (SYSTEM.mode) {
        case MODE_CLI:
            cli_ping();
            break;

        case MODE_SSMP:
            ssmp_ping();
            break;

        case MODE_UNKNOWN:
            cli_ping();
            break;
        }
    }

    if ((v & MSG) == MSG_WATCHDOG) {
        watchdog_update();
    }
}

void print(const char *msg) {
    int N = 256;
    char *s;

    if ((s = (char *)calloc(N, sizeof(char))) != NULL) {
        snprintf(s, N, "%s", msg);
        _push(s);
        _flush();
    }
}

void println(const char *msg) {
    int N = 256;
    char *s;

    if ((s = (char *)calloc(N, sizeof(char))) != NULL) {
        snprintf(s, N, "%s\n", msg);
        _push(s);
        _flush();
    }
}

void _push(char *msg) {
    if (queue_is_full(&SYSTEM.queue)) {
        for (int i = 0; i < 16; i++) {
            char *pending = NULL;
            if (queue_try_remove(&SYSTEM.queue, &pending)) {
                free(pending);
            }
        }

        char *dots;

        if ((dots = (char *)calloc(8, sizeof(char))) != NULL) {
            snprintf(dots, 8, "...\n", msg);
            if (!queue_try_add(&SYSTEM.queue, &dots)) {
                free(dots);
            }
        }
    }

    if (!queue_try_add(&SYSTEM.queue, &msg)) {
        free(msg);
    }
}

/* Flushes all pending messages to stdout.
 *
 */
void _flush() {
    if (SYSTEM.mode != MODE_CLI) {
        return;
    }

    if (mutex_try_enter(&SYSTEM.guard, NULL)) {
        char *pending = NULL;

        while (queue_try_remove(&SYSTEM.queue, &pending)) {
            _print(pending);
            free(pending);
        }

        mutex_exit(&SYSTEM.guard);
    }
}

/* Prints all of a message to stdout.
 *
 */
void _print(const char *msg) {
    int remaining = strlen(msg);
    int ix = 0;
    int N;

    while (remaining > 0) {
        if ((N = fwrite(&msg[ix], 1, remaining, stdout)) <= 0) {
            break;
        } else {
            remaining -= N;
            ix += N;
        }
    }

    fflush(stdout);
}
