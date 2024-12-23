#include <stdio.h>

#include "hardware/watchdog.h"

#include <trace.h>

const uint32_t SCRATCH_TRACE_ID = 1;
const uint32_t SCRATCH_TRACE_IN = 2;
const uint32_t SCRATCH_TRACE_OUT = 2;

volatile uint32_t trace_count = 0;

// Ref. https://forums.raspberrypi.com/viewtopic.php?t=354617
// int stacktrace __attribute__((section(".uninitialized_data")));

uint32_t trace_in(uint32_t id) {
    uint32_t count = ++trace_count;

    watchdog_hw->scratch[SCRATCH_TRACE_ID] = id;
    watchdog_hw->scratch[SCRATCH_TRACE_IN] = count;

    return count;
}

void trace_out(uint32_t id, uint32_t count) {
    watchdog_hw->scratch[SCRATCH_TRACE_OUT] = count;
}

void trace_dump() {
    uint32_t trace_id = watchdog_hw->scratch[SCRATCH_TRACE_ID];
    uint32_t trace_in = watchdog_hw->scratch[SCRATCH_TRACE_IN];
    uint32_t trace_out = watchdog_hw->scratch[SCRATCH_TRACE_OUT];

    printf(">>>> TRACE\n");
    printf("     ID:  %lu\n", trace_id);
    printf("     in:  %lu\n", trace_in);
    printf("     out: %lu\n", trace_out);
    printf("     ----\n");
}
