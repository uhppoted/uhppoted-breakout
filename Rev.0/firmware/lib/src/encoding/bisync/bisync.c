#include <stdlib.h>
#include <string.h>

#include <encoding/SSMP/SSMP.h>
#include <encoding/bisync/bisync.h>

extern uint16_t CRC16(uint16_t crc, const uint8_t *bytes, size_t N);

const uint8_t SOH = 1;
const uint8_t STX = 2;
const uint8_t ETX = 3;
const uint8_t ENQ = 5;
const uint8_t ACK = 6;
const uint8_t DLE = 16;
const uint8_t NAK = 21;
const uint8_t SYN = 22;

const char SYN_SYN_ACK[] = {SYN, SYN, ACK};

bool escape(uint8_t b);

void bisync_reset(struct bisync *codec) {
    memset(codec->header.data, 0, sizeof(codec->header.data));
    codec->header.ix = 0;

    memset(codec->data.data, 0, sizeof(codec->data.data));
    codec->data.ix = 0;

    memset(codec->crc.data, 0, sizeof(codec->crc.data));
    codec->crc.ix = 0;

    codec->DLE = false;
    codec->SOH = false;
    codec->STX = false;
    codec->CRC = false;
}

void bisync_decode(struct bisync *codec, const uint8_t *buffer, int N) {
    for (int i = 0; i < N; i++) {
        uint8_t ch = buffer[i];

        // ... CRC?
        if (codec->CRC) {
            if (codec->crc.ix < sizeof(codec->crc.data)) {
                codec->crc.data[codec->crc.ix++] = ch;

                if (codec->crc.ix == sizeof(codec->crc.data)) {
                    uint16_t CRC = 0xffff;
                    CRC = CRC16(CRC, codec->header.data, codec->header.ix);
                    CRC = CRC16(CRC, &STX, 1);
                    CRC = CRC16(CRC, codec->data.data, codec->data.ix);
                    CRC = CRC16(CRC, &ETX, 1);

                    uint16_t crc = 0x0000;
                    crc <<= 8;
                    crc |= codec->crc.data[0];
                    crc <<= 8;
                    crc |= codec->crc.data[1];

                    if ((CRC ^ crc) == 0x0000) {
                        if (codec->received != NULL) {
                            codec->received(codec->header.data, codec->header.ix, codec->data.data, codec->data.ix);
                        }
                    }

                    bisync_reset(codec);
                }
            } else {
                bisync_reset(codec);
            }

            continue;
        }

        // ... escaped?
        if (codec->DLE) {
            codec->DLE = false;

            if (codec->SOH) {
                if (codec->header.ix < sizeof(codec->header.data)) {
                    codec->header.data[codec->header.ix++] = ch;
                } else {
                    bisync_reset(codec);
                }
            }

            if (codec->STX) {
                if (codec->data.ix < sizeof(codec->data.data)) {
                    codec->data.data[codec->data.ix++] = ch;
                } else {
                    bisync_reset(codec);
                }
            }

            continue;
        }

        // ... normal'ish
        // SYN?
        if (ch == SYN) {
            bisync_reset(codec);
            continue;
        }

        // ENQ?
        // if (ch == ENQ && codec->ix == 0) {
        if (ch == ENQ) {
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
            codec->STX = false;
            codec->CRC = true;
            continue;
        }

        // DLE?
        if (ch == DLE) {
            codec->DLE = true;
            continue;
        }

        // ... accumulate message
        if (codec->SOH) {
            if (codec->header.ix < sizeof(codec->header.data)) {
                codec->header.data[codec->header.ix++] = ch;
            } else {
                bisync_reset(codec);
            }
        }

        if (codec->STX) {
            if (codec->data.ix < sizeof(codec->data.data)) {
                codec->data.data[codec->data.ix++] = ch;
            } else {
                bisync_reset(codec);
            }
        }
    }
}

slice bisync_encode(const uint8_t *header, int header_size, const uint8_t *data, int data_size) {
    // ... calculate message size
    int N = 2; // SYN-SYN

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
            if (escape(header[i])) {
                N += 1;
            }
        }
    }

    if (data != NULL) {
        for (int i = 0; i < data_size; i++) {
            if (escape(data[i])) {
                N += 1;
            }
        }
    }

    N += 2; // CRC

    // ... encode message
    slice m = {
        .capacity = N,
        .length = 0,
        .bytes = (uint8_t *)calloc(N, sizeof(uint8_t)),
    };

    m.bytes[m.length++] = SYN;
    m.bytes[m.length++] = SYN;

    if (header != NULL) {
        m.bytes[m.length++] = SOH;
        for (int i = 0; i < header_size; i++) {
            if (escape(header[i])) {
                m.bytes[m.length++] = DLE;
            }

            m.bytes[m.length++] = header[i];
        }
    }

    m.bytes[m.length++] = STX;

    if (data != NULL) {
        for (int i = 0; i < data_size; i++) {
            if (escape(data[i])) {
                m.bytes[m.length++] = DLE;
            }

            m.bytes[m.length++] = data[i];
        }
    }

    m.bytes[m.length++] = ETX;

    // ... CRC
    uint16_t crc = 0xffff;
    crc = CRC16(crc, header, header_size);
    crc = CRC16(crc, &STX, 1);
    crc = CRC16(crc, data, data_size);
    crc = CRC16(crc, &ETX, 1);

    m.bytes[m.length++] = (uint8_t)((crc >> 8) & 0x00ff);
    m.bytes[m.length++] = (uint8_t)((crc >> 0) & 0x00ff);

    return m;
}

bool escape(uint8_t b) {
    switch (b) {
    case SYN:
    case ENQ:
    case ACK:
    case SOH:
    case STX:
    case ETX:
    case DLE:
        return true;

    default:
        return false;
    }
}
