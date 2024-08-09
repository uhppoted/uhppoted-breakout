#include <stdlib.h>
#include <string.h>

#include <encoding/bisync/bisync.h>

const uint8_t SOH = 1;
const uint8_t STX = 2;
const uint8_t ETX = 3;
const uint8_t ENQ = 5;
const uint8_t ACK = 6;
const uint8_t DLE = 16;
const uint8_t NAK = 21;
const uint8_t SYN = 22;

const char SYN_SYN_ACK[] = {SYN, SYN, ACK};

void bisync_decode(struct bisync *codec, const uint8_t *buffer, int N) {
    for (int i = 0; i < N; i++) {
        uint8_t ch = buffer[i];

        // ... escape?
        if (codec->DLE) {
            codec->DLE = false;

            if (codec->ix < sizeof(codec->buffer)) {
                codec->buffer[codec->ix++] = ch;
            } else {
                memset(codec->buffer, 0, sizeof(codec->buffer));
                codec->ix = 0;
            }
        } else {
            // SYN?
            if (ch == SYN) {
                memset(codec->buffer, 0, sizeof(codec->buffer));
                codec->ix = 0;
                continue;
            }

            // ENQ?
            if (ch == ENQ && codec->ix == 0) {
                memset(codec->buffer, 0, sizeof(codec->buffer));
                codec->ix = 0;

                if (codec->enq != NULL) {
                    codec->enq();
                }
                continue;
            }

            // ETX?
            if (ch == ETX) {
                if (codec->ix < sizeof(codec->buffer)) {
                    codec->buffer[codec->ix++] = ch;

                    if (codec->received != NULL) {
                        codec->received(codec->buffer, codec->ix);
                    }
                }

                memset(codec->buffer, 0, sizeof(codec->buffer));
                codec->ix = 0;
                continue;
            }

            // DLE?
            if (ch == DLE) {
                codec->DLE = true;
                continue;
            }

            // ... accumulate message
            if (codec->ix < sizeof(codec->buffer)) {
                codec->buffer[codec->ix++] = ch;
            } else {
                memset(codec->buffer, 0, sizeof(codec->buffer));
                codec->ix = 0;
            }
        }
    }
}

message bisync_encode(const uint8_t *header, int header_size, const uint8_t *data, int data_size) {
    // ... calculate message size
    int N = 2;

    if (header != NULL) {
        N += 1;
        N += header_size;
    }

    N += 2;
    if (data != NULL) {
        N += data_size;
    }

    if (header != NULL) {
        for (int i = 0; i < header_size; i++) {
            switch (header[i]) {
            case SYN:
            case SOH:
            case STX:
            case ETX:
            case DLE:
                N += 1;
                break;
            }
        }
    }

    if (data != NULL) {
        for (int i = 0; i < data_size; i++) {
            switch (data[i]) {
            case SYN:
            case SOH:
            case STX:
            case ETX:
            case DLE:
                N += 1;
                break;
            }
        }
    }

    // ... encode message
    message m = {
        .data = (uint8_t *)calloc(N, sizeof(uint8_t)),
        .N = N,
    };

    uint8_t *p = m.data;

    *p++ = SYN;
    *p++ = SYN;

    if (header != NULL) {
        *p++ = SOH;
        for (int i = 0; i < header_size; i++) {
            switch (header[i]) {
            case SYN:
            case SOH:
            case STX:
            case ETX:
            case DLE:
                *p++ = DLE;
                break;
            }

            *p++ = header[i];
        }
    }

    *p++ = STX;
    if (data != NULL) {
        for (int i = 0; i < data_size; i++) {
            switch (data[i]) {
            case SYN:
            case SOH:
            case STX:
            case ETX:
            case DLE:
                *p++ = DLE;
                break;
            }

            *p++ = data[i];
        }
    }
    *p++ = ETX;

    return m;
}
