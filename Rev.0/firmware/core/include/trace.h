#pragma once

#include <stdint.h>

typedef enum {
    TRACE_UNKNOWN, // 0
    TRACE_U4_TICK, // 1
} TRACE;

extern uint32_t trace_in(uint32_t);
extern void trace_out(uint32_t, uint32_t count);
extern void trace_dump();