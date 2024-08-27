#pragma once

#include <stdbool.h>
#include <stdint.h>

struct message;

typedef void (*bisync_enq)();
typedef void (*bisync_msg)(const uint8_t *header, int header_len, const uint8_t *data, int data_len);

typedef struct bisync {
    char header[128];
    char data[512];
    int hx;
    int ix;
    bool DLE;
    bool SOH;
    bool STX;

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
extern struct message bisync_encode(const uint8_t *header, int header_len, const uint8_t *data, int data_len);
