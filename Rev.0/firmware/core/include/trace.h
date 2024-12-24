#pragma once

#include <stdint.h>

typedef enum {
    TRACE_UNKNOWN,      // 0
    TRACE_SYS_DISPATCH, // 1
    TRACE_SYS_TICK,     // 2
    TRACE_U2_TICK,      // 3
    TRACE_U3_TICK,      // 4
    TRACE_U4_TICK,      // 5
    TRACE_RTC_TICK,     // 6,
    TRACE_SSMP_RECEIVE, // 7
} TRACE;

extern uint32_t trace_in(uint32_t);
extern void trace_init();
extern void trace_out(uint32_t, uint32_t count);
extern void trace_dump();