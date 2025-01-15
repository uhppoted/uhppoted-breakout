#include <stdio.h>

#include "hardware/watchdog.h"

#include <trace.h>

const uint32_t SCRATCH_TRACE_ID = 1;
const uint32_t SCRATCH_TRACE_IN = 2;
const uint32_t SCRATCH_TRACE_OUT = 2;

const char *TRACEPOINTS[] = {
    "unknown",       // 0
    "sys::dispatch", // 1
    "sys::tick",     // 2
    "RTC::tick",     // 3
    "U2::tick",      // 4
    "U3::tick",      // 5
    "U4::tick",      // 6
    "SSMP::receive", // 7
};

volatile uint32_t trace_count = 0;

typedef struct stacktrace {
    int head;
    uint32_t stack[64];
} stacktrace;

stacktrace STACKTRACE __attribute__((section(".uninitialized_data")));

void trace_init() {
    STACKTRACE.head = 0;
}

uint32_t trace_in(uint32_t id) {
    uint32_t count = ++trace_count;

    watchdog_hw->scratch[SCRATCH_TRACE_ID] = id;
    watchdog_hw->scratch[SCRATCH_TRACE_IN] = count;

    if (STACKTRACE.head >= 0 && STACKTRACE.head < sizeof(STACKTRACE.stack)) {
        STACKTRACE.stack[STACKTRACE.head++] = id;
    }

    return count;
}

void trace_out(uint32_t id, uint32_t count) {
    watchdog_hw->scratch[SCRATCH_TRACE_OUT] = count;

    STACKTRACE.head--;
}

void trace_dump() {
    uint32_t trace_id = watchdog_hw->scratch[SCRATCH_TRACE_ID];
    uint32_t trace_in = watchdog_hw->scratch[SCRATCH_TRACE_IN];
    uint32_t trace_out = watchdog_hw->scratch[SCRATCH_TRACE_OUT];

    printf(">>>> TRACE %s\n", RELEASED);
    printf("     ID:  %lu\n", trace_id);
    printf("     in:  %lu\n", trace_in);
    printf("     out: %lu\n", trace_out);
    printf("     stacktrace: %d\n", STACKTRACE.head);

    for (int i = 0; i < STACKTRACE.head; i++) {
        uint32_t tracepoint = STACKTRACE.stack[i];
        const char *description = TRACEPOINTS[tracepoint];

        printf("                 %-2d %-2lu  %s\n", i, tracepoint, description);
    }

    printf("     ----\n");
}
