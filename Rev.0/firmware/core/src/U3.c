#include <stdio.h>

#include <pico/stdlib.h>

#include <I2C0.h>
#include <PCAL6408A.h>
#include <U3.h>
#include <breakout.h>
#include <log.h>
#include <state.h>

typedef struct IIR {
    float x₁;
    float y₁;
} IIR;

typedef struct LPF {
    float a₀;
    float a₁;
    float b₀;
    float b₁;
} LPF;

struct {
    struct {
        uint8_t state;
        IIR lpf[8];
    } inputs;

    struct {
        uint8_t state;
        IIR lpf[8];
    } unstable;

    repeating_timer_t timer;
} U3x = {
    .inputs = {
        .state = 0x00,
        .lpf = {
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
        },
    },

    .unstable = {
        .state = 0x00,
        .lpf = {
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
            {.x₁ = 0.0, .y₁ = 0.0},
        },
    },
};

const uint8_t S1 = 0x10;
const uint8_t S2 = 0x20;
const uint8_t S3 = 0x40;
const uint8_t S4 = 0x80;

const uint8_t PB1 = 0x08;
const uint8_t PB2 = 0x04;
const uint8_t PB3 = 0x02;
const uint8_t PB4 = 0x01;

const uint8_t U3_MASKS[] = {S1, S2, S3, S4, PB1, PB2, PB3, PB4};
const uint8_t U3_DOORS[] = {0x00, 0x01, 0x02, 0x04, 0x08};
const uint8_t U3_BUTTONS[] = {0x00, 0x10, 0x20, 0x40, 0x80};

const PULLUP U3_PULLUPS[8] = {
    PULLUP_UP, // PB4
    PULLUP_UP, // PB3
    PULLUP_UP, // PB2
    PULLUP_UP, // PB1

    PULLUP_UP, // S1
    PULLUP_UP, // S2
    PULLUP_UP, // S3
    PULLUP_UP, // S4
};

// 25Hz 1 pole Butterworth LPF (approx. 15ms debounce delay) for inputs
const struct LPF LPF₁ = {
    .a₀ = 1.0,
    .a₁ = -0.85408069,
    .b₀ = 0.07295966,
    .b₁ = 0.07295966,
};

// 1Hz 1 pole Butterworth LPF for input stability monitor
const struct LPF LPF₂ = {
    .a₀ = 1.0,
    .a₁ = -0.99937188,
    -0.99937188,
    .b₀ = 0.00031406,
    .b₁ = 0.00031406,
};

bool U3_on_update(repeating_timer_t *rt);
void U3_read(void *data);
float lpf₁(IIR *iir, float in);
float lpf₂(IIR *iir, float in);

void U3_setup() {
    infof("U3", "setup");

    // ... configure PCAL6408A
    int err;

    if ((err = PCAL6408A_init(U3)) != ERR_OK) {
        warnf("U3", "error initialising PCAL6408A (%d)", err);
    }

    if ((err = PCAL6408A_set_configuration(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error configuring PCAL6408A (%d)", err);
    }

    if ((err = PCAL6408A_set_polarity(U3, PB1 | PB2 | PB3 | PB4)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408A polarity (%d)", err);
    }

    if ((err = PCAL6408A_set_pullups(U3, U3_PULLUPS)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408A pullups (%d)", err);
    }

    if ((err = PCAL6408A_set_latched(U3, 0x00)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408A latches (%d)", err);
    }

    if ((err = PCAL6408A_set_interrupts(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error disabling PCAL6408A interrupts (%d)", err);
    }

    uint8_t inputs;
    PCAL6408A_read(U3, &inputs); // clear any existing interrupts

    debugf("U3", "initial state %02x %08b", inputs, inputs);
    infof("U3", "initialised");
}

// Starts the 1kHz input sampling
void U3_start() {
    infof("U3", "start");

    add_repeating_timer_us(1000, U3_on_update, NULL, &U3x.timer);
}

bool U3_on_update(repeating_timer_t *rt) {
    closure task = {
        .f = U3_read,
        .data = &U3x,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "U3", "update: queue full");
    }

    return true;
}

/*
 * I2C0 task to update U3 state from PCAL6408A.
 */
void U3_read(void *data) {
    uint8_t inputs;
    int err;

    if ((err = PCAL6408A_read(U3, &inputs)) != ERR_OK) {
        warnf("U3", "error reading PCAL6408A inputs (%d)", err);
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
void U3_process(uint8_t data) {
    uint8_t inputs = U3x.inputs.state;
    uint8_t unstable = U3x.unstable.state;
    uint8_t mask = 0x01;

    for (int i = 0; i < 8; i++) {
        // ... input filter
        float u = (data & U3_MASKS[i]) != 0x00 ? 1.0 : 0.0;
        float v = lpf₁(&U3x.inputs.lpf[i], u);

        if (v > 0.9) {
            inputs |= mask;
        } else if (v < 0.1) {
            inputs &= ~mask;
        }

        // ... unstable input filter
        float p = ((v > 0.9) || (v < 0.1)) ? 1.0 : 0.0;
        float q = lpf₂(&U3x.unstable.lpf[i], p);

        if (q > 0.9) {
            unstable |= mask;
        } else {
            unstable &= ~mask;
        }

        mask <<= 1;
    }

    if (inputs != U3x.inputs.state) {
        U3x.inputs.state = inputs;

        debugf("U3", "inputs   %08b (%08b)", U3x.inputs.state, U3x.unstable.state);
    }

    if (unstable != U3x.unstable.state) {
        U3x.unstable.state = unstable;

        debugf("U3", "unstable %08b (%08b)", U3x.inputs.state, U3x.unstable.state);
    }
}

/*
 * Returns true if the door is open.
 *
 * Door is in the range [1..4].
 */
bool U3_get_door(uint8_t door) {
    if ((door < 1) || (door > 4)) {
        return false;
    }

    return (U3x.inputs.state & U3_DOORS[door]) != 0x00;
}

/*
 * Returns true if the button is pressed.
 *
 * Door is in the range [1..4].
 */
bool U3_get_button(uint8_t door) {
    if ((door < 1) || (door > 4)) {
        return false;
    }

    return (U3x.inputs.state & U3_BUTTONS[door]) != 0x00;
}

// 'fast' LPF for inputs
// y₀ = (b₀x₀ + b₁x₁ - a₁y₁)/a₀
float lpf₁(IIR *iir, float x₀) {
    // clang-format off
    float y₀ = LPF₁.b₀ * x₀;
          y₀ += LPF₁.b₁ * iir->x₁;
          y₀ -= LPF₁.a₁ * iir->y₁;
    // clang-format on

    iir->x₁ = x₀;
    iir->y₁ = y₀;

    return y₀;
}

// asymmetric LPF for input instability
float lpf₂(IIR *iir, float x₀) {
    float y₀;

    if (x₀ < 1.0) {
        y₀ = 0.0;
    } else {
        // clang-format off
        y₀ = LPF₂.b₀ * x₀;
        y₀ += LPF₂.b₁ * iir->x₁;
        y₀ -= LPF₂.a₁ * iir->y₁;
        // clang-format on
    }

    iir->x₁ = x₀;
    iir->y₁ = y₀;

    return y₀;
}
