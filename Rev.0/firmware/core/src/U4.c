#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C0.h>
#include <PCAL6416A.h>
#include <breakout.h>
#include <log.h>
#include <mempool.h>

#include <U4.h>
#include <types/operation.h>

#define LOGTAG "U4"

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

const int32_t U4_TICK = 10;   // ms
const int32_t U4_TOCK = 1000; // ms
const int ID_ERR = -1;
const int ID_IN = -2;
const int ID_SYS = -3;

const PULLUP U4_PULLUPS[16] = {
    PULLUP_NONE, // relay 1
    PULLUP_NONE, // LED 1
    PULLUP_NONE, // relay 2
    PULLUP_NONE, // LED 2

    PULLUP_NONE, // relay 3
    PULLUP_NONE, // LED 3
    PULLUP_NONE, // relay 4
    PULLUP_NONE, // LED 4

    PULLUP_NONE, // LED ERR
    PULLUP_NONE, // LED IN
    PULLUP_NONE, // LED SYS
    PULLUP_NONE, // -- unused --

    PULLUP_NONE, // -- unused --
    PULLUP_NONE, // -- unused --
    PULLUP_NONE, // -- unused --
    PULLUP_NONE, // -- unused --
};

const float U4_OUTPUT_DRIVE[16] = {
    0.25f, // relay 1 (drives 5V/(1k || 10.470k) = 4.5mA)
    0.25f, // LED 1   (TTL/high impedance input)
    0.25f, // relay 2 (drives 5V/(1k || 10.470k) = 4.5mA)
    0.25f, // LED 2   (TTL/high impedance input)

    0.25f, // relay 3 (drives 5V/(1k || 10.470k) = 4.5mA)
    0.25f, // LED 3   (TTL/high impedance input)
    0.25f, // relay 4 (drives 5V/(1k || 10.470k) = 4.5mA)
    0.25f, // LED 4   (TTL/high impedance input)

    0.25f, // LED ERR (sinks 5V/1k = 5mA)
    0.25f, // LED IN  (sinks 5V/1k = 5mA)
    0.25f, // LED SYS (sinks 5V/1k = 5mA)
    0.f,   // -- unused --

    0.f, // -- unused --
    0.f, // -- unused --
    0.f, // -- unused --
    0.f, // -- unused --
};

inline void U4_set(uint16_t mask);
inline void U4_clear(uint16_t mask);
inline void U4_toggle(uint16_t mask);
inline int32_t clamp(int32_t v, int32_t min, int32_t max);

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
    uint16_t state;
    uint16_t outputs;
    uint16_t polarity;
    int32_t tock;
    bool write;

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
    .state = 0x0000,
    .outputs = 0x0000,
    .polarity = 0x0700, // SYS, IN and ERR LEDs are inverted
    .tock = U4_TOCK,
    .write = false,
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

void U4_write(void *data);
void U4_healthcheck(void *data);
bool U4_tick(repeating_timer_t *rt);

void U4_init() {
    infof(LOGTAG, "init");

    mutex_init(&U4x.guard);

    // ... configure PCAL6416A
    int delay = 100;
    int err;

    sleep_ms(delay);

    if ((err = PCAL6416A_init(U4)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error initialising (%d)", err);
    }

    sleep_ms(delay);

    uint16_t configuration;
    if ((err = PCAL6416A_get_configuration(U4, &configuration)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error reading PCAL6416A configuration (%d)", err);
    } else {
        debugf(LOGTAG, "PCAL6416A configuration (0): %04x", configuration);
    }

    sleep_ms(delay);

    if ((err = PCAL6416A_set_open_drain(U4, false, false)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error configuring PCAL6416A open drain outputs (%d)", err);
    }

    sleep_ms(delay);

    if ((err = PCAL6416A_set_polarity(U4, 0x0000)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error setting PCAL6416A polarity (%d)", err);
    }

    sleep_ms(delay);

    if ((err = PCAL6416A_set_latched(U4, 0x0000)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error setting PCAL6416A latches (%d)", err);
    }

    sleep_ms(delay);

    if ((err = PCAL6416A_set_pullups(U4, U4_PULLUPS)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error setting PCAL6416A pullups (%d)", err);
    }

    sleep_ms(delay);

    if ((err = PCAL6416A_set_output_drive(U4, U4_OUTPUT_DRIVE)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error setting PCAL6416A output drive (%d)", err);
    }

    sleep_ms(delay);

    // if ((err = PCAL6416A_write(U4, (U4x.outputs ^ U4x.polarity) & MASK)) != ERR_OK) {
    //     syserr_set(ERR_U4, LOGTAG, "error initialising PCAL6416A outputs (%d)", err);
    // }
    //
    // sleep_ms(delay);

    // if ((err = PCAL6416A_set_configuration(U4, 0xf800)) != ERR_OK) {
    //     syserr_set(ERR_U4, LOGTAG, "error configuring PCAL6416A (%d)", err);
    // }
    //
    // sleep_ms(delay);

    // if ((err = PCAL6416A_get_configuration(U4, &configuration)) != ERR_OK) {
    //     syserr_set(ERR_U4, LOGTAG, "error reading PCAL6416A configuration (%d)", err);
    // } else if (configuration != 0xf80) {
    //     syserr_set(ERR_U4, LOGTAG, "invalid PCAL6416A configuration - expected:%04x, got:%04x", 0xf800, configuration);
    // } else {
    //     debugf(LOGTAG, "PCAL6416A configuration (2): %04x", configuration);
    // }
    //
    // sleep_ms(delay);

    // uint16_t outputs;
    // if ((err = PCAL6416A_readback(U4, &outputs)) != ERR_OK) {
    //     syserr_set(ERR_U4, LOGTAG, "error reading PCAL6416A outputs (%d)", err);
    // } else if ((outputs & MASK) != ((U4x.outputs ^ U4x.polarity) & MASK)) {
    //     syserr_set(ERR_U4, LOGTAG, "invalid PCAL6416A output state - expected:%04x, got:%04x", U4x.outputs, outputs);
    // }

    // debugf(LOGTAG, "initialised state %04x %011b", outputs ^ U4x.polarity, outputs ^ U4x.polarity);
}

void U4_start() {
    infof(LOGTAG, "start");

    // add_repeating_timer_ms(U4_TICK, U4_tick, NULL, &U4x.timer);
}

/*
 * Decrements relay and LED timers.
 */
bool U4_tick(repeating_timer_t *rt) {
    uint16_t outputs = U4x.outputs;

    if (mutex_try_enter(&U4x.guard, NULL)) {
        // ... health check
        U4x.tock -= U4_TICK;
        if (U4x.tock < 0) {
            operation *op = operation_alloc();

            if (op != NULL) {
                op->tag = U4_HEALTHCHECK;
                op->healthcheck.outputs = (U4x.outputs ^ U4x.polarity) & MASK;

                struct closure task = {
                    .f = U4_healthcheck,
                    .data = op,
                };

                if (I2C0_push(&task)) {
                    U4x.tock = U4_TOCK;
                } else {
                    operation_free(op);
                }
            }
        }

        // ... update relays
        for (struct relay *r = U4x.relays.relays; r < U4x.relays.relays + U4x.relays.N; r++) {
            if (r->timer > 0) {
                r->timer = clamp(r->timer - U4_TICK, 0, 60000);
                if (r->timer == 0) {
                    U4_clear(r->mask);
                }
            }
        }

        // ... update LEDs
        for (struct LED *l = U4x.LEDs.LEDs; l < U4x.LEDs.LEDs + U4x.LEDs.N; l++) {
            if (l->timer > 0) {
                l->timer = clamp(l->timer - U4_TICK, 0, 60000);
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

        // ... update outputs
        if (outputs != U4x.outputs || U4x.write) {
            operation *op = operation_alloc();

            if (op != NULL) {
                outputs = U4x.outputs;

                op->tag = U4_WRITE;
                op->write.outputs = (outputs ^ U4x.polarity) & MASK;

                struct closure task = {
                    .f = U4_write,
                    .data = op,
                };

                if (!I2C0_push(&task)) {
                    operation_free(op);
                } else {
                    U4x.write = false;
                }
            }
        }

        mutex_exit(&U4x.guard);
    }

    return true;
}

/*
 * I2C0 task to set/clear PCAL6416A outputs.
 */
void U4_write(void *data) {
    operation *op = (operation *)data;

    uint16_t outputs = op->write.outputs;
    uint16_t state = U4x.state;
    uint16_t relays = 0x0055;
    int err;

    if ((err = PCAL6416A_write(U4, outputs & MASK)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error writing PCAL6416A outputs (%d)", err);
    } else if ((err = PCAL6416A_readback(U4, &outputs)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error reading back PCAL6416A outputs (%d)", err);
    } else if ((outputs & MASK) != (op->write.outputs & MASK)) {
        syserr_set(ERR_U4, LOGTAG, "invalid PCAL6416A output state - expected:04x, got:%04x", op->write.outputs & MASK, outputs & MASK);
    }

    operation_free(op);

    // ... door lock events
    static const struct {
        uint16_t mask;
        EVENT unlocked;
        EVENT locked;
    } events[] = {
        {0x0001, EVENT_DOOR_1_UNLOCKED, EVENT_DOOR_1_LOCKED},
        {0x0004, EVENT_DOOR_2_UNLOCKED, EVENT_DOOR_2_LOCKED},
        {0x0010, EVENT_DOOR_3_UNLOCKED, EVENT_DOOR_3_LOCKED},
        {0x0040, EVENT_DOOR_4_UNLOCKED, EVENT_DOOR_4_LOCKED},
    };

    if ((outputs & relays) != state) {
        U4x.state = outputs & relays;

        int N = sizeof(events) / sizeof(events[0]);

        for (int i = 0; i < N; i++) {
            uint8_t mask = events[i].mask;

            if (((outputs & mask) == 0x00) && ((state & mask) == mask)) {
                push((message){
                    .message = MSG_EVENT,
                    .tag = MESSAGE_EVENT,
                    .event = events[i].locked,
                });
            }

            if (((outputs & mask) == mask) && ((state & mask) == 0x00)) {
                push((message){
                    .message = MSG_EVENT,
                    .tag = MESSAGE_EVENT,
                    .event = events[i].unlocked,
                });
            }
        }
    }
}

/*
 * I2C0 task to readback and validate PCAL6416A outputs.
 */
void U4_healthcheck(void *data) {
    operation *op = (operation *)data;
    uint16_t outputs;
    int err;

    if ((err = PCAL6416A_readback(U4, &outputs)) != ERR_OK) {
        syserr_set(ERR_U4, LOGTAG, "error reading back PCAL6416A outputs (%d)", err);
    } else if ((outputs & MASK) != (op->healthcheck.outputs & MASK)) {
        syserr_set(ERR_U4, LOGTAG, "PCAL6416A healthcheck: expected:%04x, got:%04x", op->healthcheck.outputs, outputs);

        if (mutex_try_enter(&U4x.guard, NULL)) {
            U4x.write = true;
            mutex_exit(&U4x.guard);
        }
    }

    operation_free(op);
}

bool U4_get_relay(int relay) {
    for (struct relay *r = U4x.relays.relays; r < U4x.relays.relays + U4x.relays.N; r++) {
        if (r->id == relay) {
            return r->mask & U4x.outputs != 0;
        }
    }

    return false;
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
            // FIXME handle inverted polarity for SYS, IN and ERR
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

void U4_blink_ERR(int count, uint16_t interval) {
    U4_blink_LED(ID_ERR, count, interval);
}

void U4_set_IN() {
    U4_set_LED(ID_IN);
}

void U4_clear_IN() {
    U4_clear_LED(ID_IN);
}

void U4_blink_IN(int count, uint16_t interval) {
    U4_blink_LED(ID_IN, count, interval);
}

void U4_set_SYS() {
    U4_set_LED(ID_SYS);
}

void U4_clear_SYS() {
    U4_clear_LED(ID_SYS);
}

void U4_blink_SYS(int count, uint16_t interval) {
    U4_blink_LED(ID_SYS, count, interval);
}

inline void U4_set(uint16_t mask) {
    U4x.outputs |= mask;
    U4x.write = true;

    printf(">>>>>>>>>>>>>>>> SET %04x", mask);
}

inline void U4_clear(uint16_t mask) {
    U4x.outputs &= ~mask;
    U4x.write = true;
}

inline void U4_toggle(uint16_t mask) {
    uint16_t outputs = U4x.outputs;

    // clang-format off
    uint16_t v = (outputs ^ mask);
             v &= mask;
             v |= outputs & ~mask;
    // clang-format on

    U4x.outputs = v;
    U4x.write = true;
}

inline int32_t clamp(int32_t v, int32_t min, int32_t max) {
    return v < min ? min : (v > max ? max : v);
}