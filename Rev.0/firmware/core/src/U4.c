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

const int32_t TICK = 10;
const int ID_ERR = -1;
const int ID_IN = -2;
const int ID_SYS = -3;

void U4_write(void *data);
void U4_set(uint16_t mask);
void U4_clear(uint16_t mask);
void U4_toggle(uint16_t mask);
bool U4_tick(repeating_timer_t *rt);

int32_t clamp(int32_t v, int32_t min, int32_t max);

typedef enum {
    U4_UNKNOWN,
    U4_SET,
    U4_CLEAR,
    U4_TOGGLE,
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
    };
} operation;

typedef struct relay {
    int id;
    uint16_t mask;
    int32_t timer;
} relay;

typedef struct LED {
    int id;
    uint16_t mask;
    int32_t timer;
    int32_t blinks;
    int32_t interval;
} LED;

struct {
    struct {
        int N;
        relay relays[4];
    } relays;

    struct {
        int N;
        LED LEDs[7];
    } LEDs;

    repeating_timer_t timer;
    mutex_t guard;
} U4x = {
    .relays = {
        .N = sizeof(U4x.relays) / sizeof(struct relay),
        .relays = {
            {.id = 1, .mask = RELAY1, .timer = 0},
            {.id = 2, .mask = RELAY2, .timer = 0},
            {.id = 3, .mask = RELAY3, .timer = 0},
            {.id = 4, .mask = RELAY4, .timer = 0},
        },
    },
    .LEDs = {
        .N = sizeof(U4x.LEDs) / sizeof(struct LED),
        .LEDs = {
            {.id = 1, .mask = LED1, .timer = 0, .blinks = 0, .interval = 0},
            {.id = 2, .mask = LED2, .timer = 0, .blinks = 0, .interval = 0},
            {.id = 3, .mask = LED3, .timer = 0, .blinks = 0, .interval = 0},
            {.id = 4, .mask = LED4, .timer = 0, .blinks = 0, .interval = 0},
            {.id = ID_ERR, .mask = ERR, .timer = 0, .blinks = 0, .interval = 0},
            {.id = ID_IN, .mask = IN, .timer = 0, .blinks = 0, .interval = 0},
            {.id = ID_SYS, .mask = SYS, .timer = 0, .blinks = 0, .interval = 0},
        },
    },
};

void U4_init() {
    infof("U4", "init");

    // ... configure PI4IOE5V6416
    int err;

    if ((err = PI4IOE5V6416_init(U4)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error initialising (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_open_drain(U4, false, false)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error configuring PI4IOE5V6416 open drain outputs (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_polarity(U4, 0x0000)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error setting PI4IOE5V6416 polarity (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_latched(U4, 0x0000)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error setting PI4IOE5V6416 latches (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_pullups(U4, 0x00)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error setting PI4IOE5V6416 pullups (%d)", err);
    }

    uint16_t outputs = 0x0000;
    if ((err = PI4IOE5V6416_write(U4, outputs & MASK)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error setting PI4IOE5V6416 outputs (%d)", err);
    }

    if ((err = PI4IOE5V6416_readback(U4, &outputs)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error reading PI4IOE5V6416 outputs (%d)", err);
    }

    if ((err = PI4IOE5V6416_set_configuration(U4, 0xf800)) != ERR_OK) {
        set_error(ERR_U4, "U4", "error configuring PI4IOE5V6416 (%d)", err);
    }

    mutex_init(&U4x.guard);
    add_repeating_timer_ms(TICK, U4_tick, NULL, &U4x.timer);

    debugf("U4", "initialised state %04x %016b", outputs, outputs);
}

/*
 * Decrements relay and LED timers.
 */
bool U4_tick(repeating_timer_t *rt) {
    if (mutex_try_enter(&U4x.guard, NULL)) {
        for (struct relay *r = U4x.relays.relays; r < U4x.relays.relays + U4x.relays.N; r++) {
            if (r->timer > 0) {
                r->timer = clamp(r->timer - TICK, 0, 60000);
                if (r->timer == 0) {
                    U4_clear(r->mask);
                }
            }
        }

        for (struct LED *l = U4x.LEDs.LEDs; l < U4x.LEDs.LEDs + U4x.LEDs.N; l++) {
            if (l->timer > 0) {
                l->timer = clamp(l->timer - TICK, 0, 60000);
                if (l->timer == 0) {
                    if ((l->blinks > 0) && (l->interval > 0)) {
                        l->blinks--;
                        l->timer = l->interval;
                        U4_toggle(l->mask);
                    } else {
                        U4_clear(l->mask);
                    }
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

    for (struct relay *r = U4x.relays.relays; r < U4x.relays.relays + U4x.relays.N; r++) {
        if (r->id == relay) {
            U4_set(r->mask);
            r->timer = clamp(delay, 0, 60000);
        }
    }

    mutex_exit(&U4x.guard);
}

void U4_clear_relay(int relay) {
    mutex_enter_blocking(&U4x.guard);

    for (struct relay *r = U4x.relays.relays; r < U4x.relays.relays + U4x.relays.N; r++) {
        if (r->id == relay) {
            U4_clear(r->mask);
            r->timer = 0;
        }
    }

    mutex_exit(&U4x.guard);
}

void U4_set_LED(int LED) {
    mutex_enter_blocking(&U4x.guard);

    for (struct LED *l = U4x.LEDs.LEDs; l < U4x.LEDs.LEDs + U4x.LEDs.N; l++) {
        if (l->id == LED) {
            U4_set(l->mask);
            l->timer = 0;
        }
    }

    mutex_exit(&U4x.guard);
}

void U4_clear_LED(int LED) {
    mutex_enter_blocking(&U4x.guard);

    for (struct LED *l = U4x.LEDs.LEDs; l < U4x.LEDs.LEDs + U4x.LEDs.N; l++) {
        if (l->id == LED) {
            U4_clear(l->mask);
            l->timer = 0;
        }
    }

    mutex_exit(&U4x.guard);
}

void U4_toggle_LED(int LED) {
    mutex_enter_blocking(&U4x.guard);

    for (struct LED *l = U4x.LEDs.LEDs; l < U4x.LEDs.LEDs + U4x.LEDs.N; l++) {
        if (l->id == LED) {
            U4_toggle(l->mask);
            l->timer = 0;
        }
    }

    mutex_exit(&U4x.guard);
}

// NTS: adds the number of blinks to the current 'blink count'
void U4_blink_LED(int LED, int count, uint16_t interval) {
    mutex_enter_blocking(&U4x.guard);

    for (struct LED *l = U4x.LEDs.LEDs; l < U4x.LEDs.LEDs + U4x.LEDs.N; l++) {
        if (l->id == LED) {
            l->timer = interval;
            l->interval = 1000;
            l->blinks = clamp(l->blinks + 2 * count, 0, 64);
        }
    }

    mutex_exit(&U4x.guard);
}

void U4_set_ERR() {
    U4_set_LED(ID_ERR);
}

void U4_clear_ERR() {
    U4_clear_LED(ID_ERR);
}

void U4_set_IN() {
    U4_set_LED(ID_IN);
}

void U4_clear_IN() {
    U4_clear_LED(ID_IN);
}

void U4_set_SYS() {
    U4_set_LED(ID_SYS);
}

void U4_clear_SYS() {
    U4_clear_LED(ID_SYS);
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

int32_t clamp(int32_t v, int32_t min, int32_t max) {
    return v < min ? min : (v > max ? max : v);
}