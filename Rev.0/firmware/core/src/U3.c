#include <stdio.h>

#include <pico/stdlib.h>

#include <I2C0.h>
#include <PCAL6408APW.h>
#include <U3.h>
#include <breakout.h>
#include <log.h>
#include <state.h>

bool U3_on_update(repeating_timer_t *rt);
void U3_get(void *data);

struct {
    uint8_t inputs;
    repeating_timer_t timer;
} U3x = {
    .inputs = 0x00,
};

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
    } else if (inputs != U3x.inputs) {
        U3x.inputs = inputs;

        uint32_t v = U3x.inputs;
        uint32_t msg = MSG_INPUTS | (v & 0x0fffffff);

        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "U3", "get: queue full");
        }
    }
}
