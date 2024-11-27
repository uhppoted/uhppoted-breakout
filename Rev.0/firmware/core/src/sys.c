#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <hardware/flash.h>
#include <hardware/watchdog.h>
#include <pico/stdlib.h>
#include <pico/sync.h>
#include <pico/unique_id.h>

#include <SSMP.h>
#include <U2.h>
#include <U3.h>
#include <breakout.h>
#include <cli.h>
#include <log.h>
#include <state.h>
#include <sys.h>

#define PRINT_QUEUE_SIZE 64

extern const char *TERMINAL_QUERY_STATUS;
const int32_t FLUSH = 1000;              // ms
const uint32_t MODE_CLI_TIMEOUT = 15000; // ms

const uint32_t MSG = 0xf0000000;
const uint32_t MSG_DEBUG = 0x00000000;
const uint32_t MSG_WIO = 0x10000000;
const uint32_t MSG_U3 = 0x20000000;
const uint32_t MSG_RX = 0x30000000;
const uint32_t MSG_TTY = 0xc0000000;
const uint32_t MSG_LOG = 0xd0000000;
const uint32_t MSG_WATCHDOG = 0xe0000000;
const uint32_t MSG_TICK = 0xf0000000;

struct {
    mode mode;

    struct {
        int head;
        int tail;
        char list[PRINT_QUEUE_SIZE][128];
    } queue;

    struct {
        absolute_time_t mode;
    } touched;

    mutex_t guard;
} SYSTEM = {
    .mode = MODE_UNKNOWN,
    .queue = {
        .head = 0,
        .tail = 0,
    },
    .touched = {
        .mode = 0,
    }};

void _push(const char *);
void _flush();
void _print(const char *);

void sysinit() {
    mutex_init(&SYSTEM.guard);

    set_mode(MODE_UNKNOWN);
}

bool sys_on_tick(repeating_timer_t *t) {
    uint32_t msg = MSG_TICK;
    if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
        set_error(ERR_QUEUE_FULL, "SYS", "tick: queue full");
    }

    return true;
}

int sys_id(char *ID, int N) {
    pico_unique_board_id_t board_id;

    pico_get_unique_board_id(&board_id);

    snprintf(ID, N, "%02x%02x%02x%02x%02x%02x%02x%02x",
             board_id.id[0],
             board_id.id[1],
             board_id.id[2],
             board_id.id[3],
             board_id.id[4],
             board_id.id[5],
             board_id.id[6],
             board_id.id[7]);

    return N < 16 ? N : 16;
}

mode get_mode() {
    switch (SYSTEM.mode) {
    case MODE_CLI:
        return MODE_CLI;

    default:
        return MODE_UNKNOWN;
    }
}

void set_mode(mode mode) {
    if (SYSTEM.mode != mode) {
        SYSTEM.mode = mode;
    }

    // ... unblock queue
    if (mode == MODE_CLI) {
        SYSTEM.touched.mode = get_absolute_time();
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
        struct circular_buffer *b = (struct circular_buffer *)(SRAM_BASE | (v & 0x0fffffff));

        SSMP_rx(b);
    }

    if ((v & MSG) == MSG_TTY) {
        struct circular_buffer *b = (struct circular_buffer *)(SRAM_BASE | (v & 0x0fffffff));

        cli_rx(b);
    }

    if ((v & MSG) == MSG_TICK) {
        sys_tick();

        // ... MODE_CLI timeout?
        absolute_time_t now = get_absolute_time();
        int64_t delta = absolute_time_diff_us(SYSTEM.touched.mode, now) / 1000;

        if (llabs(delta) > MODE_CLI_TIMEOUT) {
            set_mode(MODE_UNKNOWN);
        }

        // ... ping terminal
        if (mutex_try_enter(&SYSTEM.guard, NULL)) {
            _print(TERMINAL_QUERY_STATUS);
            mutex_exit(&SYSTEM.guard);
        }

        // ... bump log queue
        uint32_t m = MSG_LOG;
        if (queue_is_full(&queue) || !queue_try_add(&queue, &m)) {
            set_error(ERR_QUEUE_FULL, "SYS", "log: queue full");
        }
    }

    if ((v & MSG) == MSG_LOG) {
        _flush();
    }

    if ((v & MSG) == MSG_WATCHDOG) {
        watchdog_update();
        sys_watchdog_update();
    }
}

void print(const char *msg) {
    _push(msg);
}

void _push(const char *msg) {
    int head = SYSTEM.queue.head;
    int tail = SYSTEM.queue.tail;
    int next = (head + 1) % PRINT_QUEUE_SIZE;

    if (next == SYSTEM.queue.tail) {
        if (tail == head) {
            // TODO replace tail entry with "..."
        } else {
            tail++;
            SYSTEM.queue.tail = tail % PRINT_QUEUE_SIZE;
        }
    }

    if (next != SYSTEM.queue.tail) {
        snprintf(SYSTEM.queue.list[head], 128, "%s", msg);
        SYSTEM.queue.head = next;
    }

    uint32_t m = MSG_LOG;
    if (queue_is_full(&queue) || !queue_try_add(&queue, &m)) {
        set_error(ERR_QUEUE_FULL, "SYS", "log: queue full");
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
        int head = SYSTEM.queue.head;
        int tail = SYSTEM.queue.tail;

        while (tail != head) {
            _print(SYSTEM.queue.list[tail++]);
            tail %= PRINT_QUEUE_SIZE;
        }

        SYSTEM.queue.tail = tail;

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

uint32_t get_total_heap() {
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

uint32_t get_free_heap() {
    extern char __StackLimit, __bss_end__;

    uint32_t total = &__StackLimit - &__bss_end__;
    struct mallinfo m = mallinfo();

    return total - m.uordblks;
}
