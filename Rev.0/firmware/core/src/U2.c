#include <PCAL6408APW.h>

#include <U2.h>
#include <breakout.h>
#include <log.h>

void U2_init() {
    infof("U2", "init");

    int err;

    if ((err = PCAL6408APW_init(U2)) != ERR_OK) {
        warnf("U2", "error initialising PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_configuration(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error configuring PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_polarity(U2, 0x00)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408APW polarity (%d)", err);
    }

    if ((err = PCAL6408APW_set_latched(U2, 0x00)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408APW latches (%d)", err);
    }

    if ((err = PCAL6408APW_set_pullups(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408APW pullups (%d)", err);
    }

    infof("U2", "initialised");
}

void U2_debug() {
    infof("U2", "debug");

    uint8_t inputs;
    int err;

    for (int i = 0; i < 30; i++) {
        if ((err = PCAL6408APW_read(U2, &inputs)) != ERR_OK) {
            warnf("U2", "error reading PCAL6408APW inputs (%d)", err);
        } else {
            infof("U2", "INPUTS %02x %08b", inputs, inputs);
        }

        sleep_ms(1000);
    }
}
