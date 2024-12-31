#pragma once

#include <stdint.h>

typedef enum {
    TRACE_UNKNOWN,        // 0
    TRACE_SYS_DISPATCH,   // 1
    TRACE_SYS_TICK,       // 2
    TRACE_U2_TICK,        // 3
    TRACE_U3_TICK,        // 4
    TRACE_U4_TICK,        // 5
    TRACE_U4_HEALTHCHECK, // 6
    TRACE_U4_RELAYS,      // 7
    TRACE_U4_LEDS,        // 8
    TRACE_U4_OUTPUTS,     // 9
    TRACE_U4_CALLOC,      // 10
    TRACE_U4_SET_ERROR,   // 11
    TRACE_U4_FREE,        // 12
    TRACE_RTC_TICK,       // 13
    TRACE_SSMP_RECEIVE,   // 14
} TRACE;

extern uint32_t trace_in(uint32_t);
extern void trace_init();
extern void trace_out(uint32_t, uint32_t count);
extern void trace_dump();