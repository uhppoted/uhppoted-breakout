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

void bisync_reset(struct bisync *codec) {
    memset(codec->header, 0, sizeof(codec->header));
    memset(codec->data, 0, sizeof(codec->data));
    codec->hx = 0;
    codec->ix = 0;
    codec->SOH = false;
    codec->STX = false;
}

void bisync_decode(struct bisync *codec, const uint8_t *buffer, int N) {
    for (int i = 0; i < N; i++) {
        uint8_t ch = buffer[i];

        // ... escape?
        if (codec->DLE) {
            codec->DLE = false;

            if (codec->SOH) {
                if (codec->hx < sizeof(codec->header)) {
                    codec->header[codec->hx++] = ch;
                } else {
                    bisync_reset(codec);
                }
            }

            if (codec->STX) {
                if (codec->ix < sizeof(codec->data)) {
                    codec->data[codec->ix++] = ch;
                } else {
                    bisync_reset(codec);
                }
            }
        } else {
            // SYN?
            if (ch == SYN) {
                bisync_reset(codec);
                continue;
            }

            // ENQ?
            if (ch == ENQ && codec->ix == 0) {
                bisync_reset(codec);
                if (codec->enq != NULL) {
                    codec->enq();
                }
                continue;
            }

            // SOH?
            if (ch == SOH) {
                bisync_reset(codec);
                codec->SOH = true;
                continue;
            }

            // STX?
            if (ch == STX) {
                codec->SOH = false;
                codec->STX = true;
                continue;
            }

            // ETX?
            if (ch == ETX) {
                if (codec->ix < sizeof(codec->data)) {
                    if (codec->received != NULL) {
                        codec->received(codec->header, codec->hx, codec->data, codec->ix);
                    }
                }

                bisync_reset(codec);
                continue;
            }

            // DLE?
            if (ch == DLE) {
                codec->DLE = true;
                continue;
            }

            // ... accumulate message
            if (codec->SOH) {
                if (codec->hx < sizeof(codec->header)) {
                    codec->header[codec->hx++] = ch;
                } else {
                    bisync_reset(codec);
                }
            }

            if (codec->STX) {
                if (codec->ix < sizeof(codec->data)) {
                    codec->data[codec->ix++] = ch;
                } else {
                    bisync_reset(codec);
                }
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
