#include <stdio.h>

#include <pico/stdlib.h>

#include <PI4IOE5V6416.h>
#include <U4.h>
#include <breakout.h>
#include <log.h>

void U4_init() {
    infof("U4", "init");

    // ... configure PI4IOE5V6416
    int err;

    if ((err = PI4IOE5V6416_init(U4)) != ERR_OK) {
        warnf("U4", "error initialising PI4IOE5V6416 (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_configuration(U4, 0xf800)) != ERR_OK) {
        warnf("U4", "error configuring PI4IOE5V6416 (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_polarity(U4, 0x0000)) != ERR_OK) {
        warnf("U4", "error setting PI4IOE5V6416 polarity (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_latched(U4, 0x0000)) != ERR_OK) {
        warnf("U4", "error setting PI4IOE5V6416 latches (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_pullups(U4, 0x00)) != ERR_OK) {
        warnf("U4", "error setting PI4IOE5V6416 pullups (%d)", err);
    }

    uint16_t outputs = 0x0000;
    if ((err = PI4IOE5V6416_write(U4, outputs)) != ERR_OK) {
        warnf("U4", "error setting PI4IOE5V6416 latches (%d)", err);
    }

    debugf("U4", "initial state %04x %016b", outputs, outputs);
}

void U4_debug() {
    infof("U4", "debug");

    static uint16_t OUTPUTS[] = {
        0x0000,
        0x0001,
        0x0002,
        0x0004,
        0x0008,
        0x0010,
        0x0020,
        0x0040,
        0x0080,
        0x0100,
        0x0200,
        0x0400,
        0x0000,
    };

    static int N = sizeof(OUTPUTS) / sizeof(uint16_t);
    static int ix = 0;

    uint16_t outputs = OUTPUTS[ix % N];
    int err;

    if ((err = PI4IOE5V6416_write(U4, outputs)) != ERR_OK) {
        warnf("U4", "error writing PI4IOE5V6416 outputs (%d)", err);
    }

    infof("U4", "OUTPUTS %04x %016b", outputs, outputs);

    ix++;
}
