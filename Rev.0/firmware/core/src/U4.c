#include <stdio.h>
#include <stdlib.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C0.h>
#include <PI4IOE5V6416.h>
#include <U4.h>
#include <breakout.h>
#include <log.h>
#include <state.h>

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

const uint16_t LEDS[] = {LED1, LED2, LED3, LED4};

const int32_t TICK = 10;

void U4_write(void *data);
void U4_set(uint16_t mask);
void U4_clear(uint16_t mask);
void U4_toggle(uint16_t mask);
int64_t U4_callback(alarm_id_t id, void *data);
bool U4_tick(repeating_timer_t *rt);
int32_t clamp(int32_t v, int32_t min, int32_t max);

typedef enum {
    U4_UNKNOWN,
    U4_SET,
    U4_CLEAR,
    U4_TOGGLE,
    U4_BLINK,
} U4_TASK;

typedef struct operation {
    U4_TASK tag;
    union {
        struct {
            uint16_t mask;
        } set;

        struct {
            uint16_t mask;
        } clear;

        struct {
            uint16_t mask;
        } toggle;

        struct {
            int led;
            int count;
            uint16_t interval;
        } blink;
    };
} operation;

typedef struct relay {
    int id;
    uint16_t mask;
    int32_t timer;
} relay;

struct {
    relay relays[4];
    repeating_timer_t timer;
    mutex_t guard;
} U4x = {
    .relays = {
        {.id = 1, .mask = RELAY1, .timer = 0},
        {.id = 2, .mask = RELAY2, .timer = 0},
        {.id = 3, .mask = RELAY3, .timer = 0},
        {.id = 4, .mask = RELAY4, .timer = 0},
    },
};

void U4_init() {
    infof("U4", "init");

    // ... configure PI4IOE5V6416
    int err;

    if ((err = PI4IOE5V6416_init(U4)) != ERR_OK) {
        warnf("U4", "error initialising PI4IOE5V6416 (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_open_drain(U4, false, false)) != ERR_OK) {
        warnf("U4", "error configuring PI4IOE5V6416 open drain outputs (%d)", err);
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

    mutex_init(&U4x.guard);
    add_repeating_timer_ms(TICK, U4_tick, NULL, &U4x.timer);

    debugf("U4", "initialised state %04x %016b", outputs, outputs);
}

/*
 * Decrements relay timers.
 */
bool U4_tick(repeating_timer_t *rt) {
    if (mutex_try_enter(&U4x.guard, NULL)) {
        for (int ix = 0; ix < 4; ix++) {
            relay *r = &U4x.relays[ix];

            if (r->timer > 0) {
                r->timer = clamp(r->timer - TICK, 0, 60000);
                if (r->timer == 0) {
                    U4_clear(r->mask);
                }
            }
        }

        mutex_exit(&U4x.guard);
    }

    return true;
}

/*
 * I2C0 task to set/clear PI4IOE5V6416 outputs.
 */
void U4_write(void *data) {
    operation *op = (operation *)data;

    if (op->tag == U4_SET) {
        uint16_t mask = op->set.mask;
        uint16_t outputs = 0x0000;
        int err;

        if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
            set_error(ERR_U4, "U4", "error reading PI4IOE5V6416 outputs (%d)", err);
        } else if ((err = PI4IOE5V6416_write(U4, (outputs | mask) & MASK)) != ERR_OK) {
            set_error(ERR_U4, "U4", "error setting PI4IOE5V6416 outputs (%d)", err);
        } else if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
            set_error(ERR_U4, "U4", "error reading back PI4IOE5V6416 outputs (%d)", err);
        }
    }

    if (op->tag == U4_CLEAR) {
        uint16_t mask = op->clear.mask;
        uint16_t outputs = 0x0000;
        int err;

        if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
            set_error(ERR_U4, "U4", "error reading PI4IOE5V6416 outputs (%d)", err);
        } else if ((err = PI4IOE5V6416_write(U4, (outputs & ~mask) & MASK)) != ERR_OK) {
            set_error(ERR_U4, "U4", "error clearing PI4IOE5V6416 outputs (%d)", err);
        } else if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
            set_error(ERR_U4, "U4", "error reading back PI4IOE5V6416 outputs (%d)", err);
        }
    }

    if (op->tag == U4_TOGGLE) {
        uint16_t mask = op->toggle.mask;
        uint16_t outputs = 0x0000;
        int err;

        if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
            set_error(ERR_U4, "U4", "error reading PI4IOE5V6416 outputs (%d)", err);
        } else {
            // clang-format off
            uint16_t v = (outputs ^ mask);
                     v &= mask;
                     v |= outputs & ~mask;
            // clang-format on

            if ((err = PI4IOE5V6416_write(U4, v & MASK)) != ERR_OK) {
                set_error(ERR_U4, "U4", "error toggling PI4IOE5V6416 outputs (%d)", err);
            } else if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
                set_error(ERR_U4, "U4", "error reading back PI4IOE5V6416 outputs (%d)", err);
            }
        }
    }

    free(data);
}

void U4_set_relay(int relay, uint16_t delay) {
    mutex_enter_blocking(&U4x.guard);

    for (int ix = 0; ix < 4; ix++) {
        if (U4x.relays[ix].id == relay) {
            struct relay *r = &U4x.relays[ix];

            U4_set(r->mask);
            r->timer = clamp(delay, 0, 60000);
        }
    }

    mutex_exit(&U4x.guard);
}

void U4_clear_relay(int relay) {
    mutex_enter_blocking(&U4x.guard);

    for (int ix = 0; ix < 4; ix++) {
        if (U4x.relays[ix].id == relay) {
            struct relay *r = &U4x.relays[ix];

            U4_clear(r->mask);
            r->timer = 0;
        }
    }

    mutex_exit(&U4x.guard);
}

void U4_set_LED(int LED) {
    if (LED >= 1 && LED <= 4) {
        uint16_t led = LEDS[LED - 1];

        U4_set(led);
    }
}

void U4_clear_LED(int LED) {
    if (LED >= 1 && LED <= 4) {
        uint16_t led = LEDS[LED - 1];

        U4_clear(led);
    }
}

void U4_toggle_LED(int LED) {
    if (LED >= 1 && LED <= 4) {
        uint16_t led = LEDS[LED - 1];

        U4_toggle(led);
    }
}

void U4_blink_LED(int LED, int count, uint16_t interval) {
    if (LED >= 1 && LED <= 4) {
        uint16_t led = LEDS[LED - 1];

        U4_set(led);

        operation *op = (operation *)calloc(1, sizeof(operation));
        op->tag = U4_BLINK;
        op->blink.led = LED;
        op->blink.count = 2 * count - 1;
        op->blink.interval = interval;

        add_alarm_in_ms(interval, U4_callback, op, false);
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
    operation *op = (operation *)calloc(1, sizeof(operation));

    op->tag = U4_SET;
    op->set.mask = mask;

    closure task = {
        .f = U4_write,
        .data = op,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "U4", "set: queue full");
    }
}

void U4_clear(uint16_t mask) {
    operation *op = (operation *)calloc(1, sizeof(operation));

    op->tag = U4_CLEAR;
    op->clear.mask = mask;

    closure task = {
        .f = U4_write,
        .data = op,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "U4", "clear: queue full");
    }
}

void U4_toggle(uint16_t mask) {
    operation *op = (operation *)calloc(1, sizeof(operation));

    op->tag = U4_TOGGLE;
    op->toggle.mask = mask;

    closure task = {
        .f = U4_write,
        .data = op,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "U4", "toggle: queue full");
    }
}

int64_t U4_callback(alarm_id_t id, void *data) {
    operation *op = data;

    if (op->tag == U4_BLINK) {
        int led = op->blink.led;
        U4_toggle_LED(led);

        if (--op->blink.count > 0) {
            return -1000 * (int64_t)op->blink.interval;
        }
    }

    free(data);

    return 0;
}

int32_t clamp(int32_t v, int32_t min, int32_t max) {
    return v < min ? min : (v > max ? max : v);
}