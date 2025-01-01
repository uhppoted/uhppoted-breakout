#pragma once

#include <stdint.h>

typedef enum {
    TRACE_UNKNOWN,            // 0
    TRACE_SYS_DISPATCH,       // 1
    TRACE_SYS_TICK,           // 2
    TRACE_U2_TICK,            // 3
    TRACE_U3_TICK,            // 4
    TRACE_U4_TICK,            // 5
    TRACE_U4_HEALTHCHECK,     // 6
    TRACE_U4_RELAYS,          // 7
    TRACE_U4_LEDS,            // 8
    TRACE_U4_OUTPUTS,         // 9
    TRACE_U4_CALLOC,          // 10
    TRACE_U4_SET_ERROR,       // 11
    TRACE_U4_FREE,            // 12
    TRACE_RTC_TICK,           // 13
    TRACE_SSMP_RECEIVE,       // 14
    TRACE_ASN1_DECODE,        // 15
    TRACE_ASN1_NEW,           // 16
    TRACE_ASN1_UNPACK_INT,    // 17
    TRACE_ASN1_UNPACK_OCTETS, // 18
    TRACE_ASN1_UNPACK_NULL,   // 19
    TRACE_ASN1_UNPACK_OID,    // 20
    TRACE_ASN1_UNPACK_SEQ,    // 21
    TRACE_ASN1_UNPACK_PDU,    // 22
    TRACE_ASN1_UNPACK_DEF,    // 23
    TRACE_ASN1_INT,           // 24
    TRACE_ASN1_INT_CALLOC,    // 25
    TRACE_ASN1_INT_FIELD,     // 26
    TRACE_ASN1_PDU,           // 27
    TRACE_ASN1_PDU_CALLOC,    // 28
    TRACE_ASN1_PDU_FIELDS,    // 29
    TRACE_SSMP_DECODE,        // 30
    TRACE_SSMP_BER,           // 31
    TRACE_SSMP_PACKET,        // 32
    TRACE_SSMP_GET,           // 33
    TRACE_SSMP_PACKET_FREE,   // 34
    TRACE_SSMP_VECTOR_FREE,   // 35
    TRACE_VECTOR_NEW,         // 36
    TRACE_VECTOR_FREE,        // 37
} TRACE;

extern uint32_t trace_in(uint32_t);
extern void trace_init();
extern void trace_out(uint32_t, uint32_t count);
extern void trace_dump();