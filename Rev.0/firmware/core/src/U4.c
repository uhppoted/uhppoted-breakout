#include <stdio.h>

#include <pico/stdlib.h>

#include <PI4IOE5V6416.h>
#include <U4.h>
#include <breakout.h>
#include <log.h>

const uint16_t MASK = 0x07ff;

const uint16_t RELAY1 = 0x0001;
const uint16_t RELAY2 = 0x0004;
const uint16_t RELAY3 = 0x0010;
const uint16_t RELAY4 = 0x0040;

const uint16_t LED1 = 0x0002;
const uint16_t LED2 = 0x0008;
const uint16_t LED3 = 0x0020;
const uint16_t LED4 = 0x0080;

const uint16_t ERR = 0x0100;
const uint16_t IN = 0x0200;
const uint16_t SYS = 0x0400;

const uint16_t RELAYS[] = {RELAY1, RELAY2, RELAY3, RELAY4};
const uint16_t LEDS[] = {LED1, LED2, LED3, LED4};

void U4_set(uint16_t mask);
void U4_clear(uint16_t mask);

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
    if ((err = PI4IOE5V6416_write(U4, outputs & MASK)) != ERR_OK) {
        warnf("U4", "error setting PI4IOE5V6416 outputs (%d)", err);
    }

    if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
        warnf("U4", "error reading PI4IOE5V6416 outputs (%d)", err);
    }

    debugf("U4", "initial state %04x %016b", outputs, outputs);
}

void U4_set_relay(int relay, bool state) {
    if (relay >= 1 && relay <= 4) {
        if (state) {
            U4_set(RELAYS[relay - 1]);
        } else {
            U4_clear(RELAYS[relay - 1]);
        }
    }
}

void U4_set_LED(int LED, bool state) {
    if (LED >= 1 && LED <= 4) {
        uint16_t led = LEDS[LED - 1];

        if (state) {
            U4_set(led);
        } else {
            U4_clear(led);
        }
    }
}

void U4_set_ERR(bool state) {
    if (state) {
        U4_set(ERR);
    } else {
        U4_clear(ERR);
    }
}

void U4_set_IN(bool state) {
    if (state) {
        U4_set(IN);
    } else {
        U4_clear(IN);
    }
}

void U4_set_SYS(bool state) {
    if (state) {
        U4_set(SYS);
    } else {
        U4_clear(SYS);
    }
}

void U4_set(uint16_t mask) {
    uint16_t outputs = 0x0000;
    int err;

    if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
        warnf("U4", "error reading PI4IOE5V6416 outputs (%d)", err);
    } else if ((err = PI4IOE5V6416_write(U4, (outputs | mask) & MASK)) != ERR_OK) {
        warnf("U4", "error setting PI4IOE5V6416 outputs (%d)", err);
    } else if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
        warnf("U4", "error reading PI4IOE5V6416 outputs (%d)", err);
    }
}

void U4_clear(uint16_t mask) {
    uint16_t outputs = 0x0000;
    int err;

    if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
        warnf("U4", "error reading PI4IOE5V6416 outputs (%d)", err);
    } else if ((err = PI4IOE5V6416_write(U4, (outputs & ~mask) & MASK)) != ERR_OK) {
        warnf("U4", "error setting PI4IOE5V6416 outputs (%d)", err);
    } else if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
        warnf("U4", "error reading PI4IOE5V6416 outputs (%d)", err);
    }
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
