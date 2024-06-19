#include <stdio.h>

#include <pico/stdlib.h>

#include <I2C0.h>
#include <IIR.h>
#include <PCAL6408APW.h>
#include <U3.h>
#include <breakout.h>
#include <log.h>
#include <state.h>

bool U3_on_update(repeating_timer_t *rt);
void U3_get(void *data);

struct {
    uint8_t inputs;
    IIR lpf[8];
    repeating_timer_t timer;
} U3x = {
    .inputs = 0x00,
    .lpf = {
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
        {.x = 0.0, .y = 0.0, .a = {1.0, -0.96128084}, .b = {0.01935958, 0.01935958}}, // 1Hz 1 pole Butterworth LPF
    }};

const uint8_t S1 = 0x10;
const uint8_t S2 = 0x20;
const uint8_t S3 = 0x40;
const uint8_t S4 = 0x80;

const uint8_t PB1 = 0x08;
const uint8_t PB2 = 0x04;
const uint8_t PB3 = 0x02;
const uint8_t PB4 = 0x01;

const uint8_t U3_MASKS[] = {S1, S2, S3, S4, PB1, PB2, PB3, PB4};

void U3_init() {
    infof("U3", "init");

    // ... configure PCAL6408APW
    int err;

    if ((err = PCAL6408APW_init(U3)) != ERR_OK) {
        warnf("U3", "error initialising PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_configuration(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error configuring PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_polarity(U3, 0x00)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408APW polarity (%d)", err);
    }

    if ((err = PCAL6408APW_set_pullups(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408APW pullups (%d)", err);
    }

    if ((err = PCAL6408APW_set_latched(U3, 0x00)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408APW latches (%d)", err);
    }

    if ((err = PCAL6408APW_set_interrupts(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error disabling PCAL6408APW interrupts (%d)", err);
    }

    uint8_t inputs;
    PCAL6408APW_read(U3, &inputs); // clear any existing interrupts

    debugf("U3", "initial state %02x %08b", inputs, inputs);

    // ... sample @1kHz
    add_repeating_timer_us(1000, U3_on_update, NULL, &U3x.timer);

    infof("U3", "initialised");
}

bool U3_on_update(repeating_timer_t *rt) {
    closure task = {
        .f = U3_get,
        .data = &U3x,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "U3", "update: queue full");
    }

    return true;
}

/*
 * I2C0 task to update RTC struct from RX8900SA.
 */
void U3_get(void *data) {
    uint8_t inputs;
    int err;

    if ((err = PCAL6408APW_read(U3, &inputs)) != ERR_OK) {
        warnf("U3", "error reading PCAL6408APW inputs (%d)", err);
        set_error(ERR_U3, "U3", "get-inputs error %d", err);
    } else {
        uint32_t v = inputs;
        uint32_t msg = MSG_U3 | (v & 0x0fffffff);

        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "U3", "get: queue full");
        }
    }
}

/*
 * Updates the input LPF's.
 */
void U3_process(uint8_t inputs) {
    uint8_t bits = U3x.inputs;
    uint8_t mask = 0x01;

    // debugf("U3", "---");

    for (int i = 0; i < 8; i++) {
        float u = (inputs & U3_MASKS[i]) != 0x00 ? 1.0 : 0.0;
        float v = IIR_process(&U3x.lpf[i], u);

        if (v > 0.9) {
            bits |= mask;
        } else if (v < 0.1) {
            bits &= ~mask;
        }

        mask <<= 1;

        // debugf("U3", "IIR(%d) bits:%02x u:%.3f v:%0.3f", i + 1, inputs, u, v);
    }

    if (bits != U3x.inputs) {
        debugf("U3", "current:%08b  updated:%08b", U3x.inputs, bits);

        U3x.inputs = bits;
    }
}
