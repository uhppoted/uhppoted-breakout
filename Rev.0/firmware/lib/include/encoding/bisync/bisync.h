#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*bisync_enq)();
typedef void (*bisync_msg)(const uint8_t *, int N);

typedef struct message {
    uint8_t *data;
    int N;
} message;

typedef struct bisync {
    char buffer[512];
    int ix;
    bool DLE;

    bisync_enq enq;
    bisync_msg received;
} bisync;

extern const uint8_t SOH;
extern const uint8_t STX;
extern const uint8_t ETX;
extern const uint8_t ENQ;
extern const uint8_t ACK;
extern const uint8_t DLE;
extern const uint8_t NAK;
extern const uint8_t SYN;

extern const char SYN_SYN_ACK[];

extern void bisync_decode(struct bisync *codec, const uint8_t *buffer, int N);
extern message bisync_encode(const uint8_t *header, int header_size, const uint8_t *data, int data_size);
