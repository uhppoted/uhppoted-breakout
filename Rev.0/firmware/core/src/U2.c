#include <stdio.h>
#include <stdlib.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C1.h>
#include <PCAL6408A.h>
#include <U2.h>
#include <breakout.h>
#include <log.h>
#include <state.h>
#include <trace.h>

struct reader;
struct keypad;

void U2_on_interrupt(void);
void U2_read(void);
void U2_on_card_read(uint8_t door, uint32_t v);
void U2_on_keypad_digit(uint8_t door, uint32_t v);
void U2_on_keycode(uint8_t door, const char *code, int length);
bool U2_tick(repeating_timer_t *rt);
void U2_tick_reader(uint8_t door, struct reader *reader);
void U2_tick_keypad(uint8_t door, struct keypad *keypad);
int U2_bits(uint32_t v);

const uint8_t tRST = 1;    // tRST (µs) for PCAL6408A interrupt line
const uint8_t R1D0 = 0x08; // reader 1 D0
const uint8_t R1D1 = 0x04; // reader 1 D1
const uint8_t R2D0 = 0x02; // reader 2 D0
const uint8_t R2D1 = 0x01; // reader 2 D1
const uint8_t R3D0 = 0x10; // reader 3 D0
const uint8_t R3D1 = 0x20; // reader 3 D1
const uint8_t R4D0 = 0x40; // reader 4 D0
const uint8_t R4D1 = 0x80; // reader 4 D1

const uint8_t D0[4] = {R1D0, R2D0, R3D0, R4D0};
const uint8_t D1[4] = {R1D1, R2D1, R3D1, R4D1};
const int32_t U2_TICK = 1;            // ms
const int32_t U2_READ_TIMEOUT = 10;   // ms
const int32_t U2_CODE_TIMEOUT = 2500; // ms
const int32_t U2_CARD_LOCK = 1250;    // ms
const int32_t U2_CODE_LOCK = 1250;    // ms

typedef struct reader {
    uint64_t data;
    uint8_t count;
    uint8_t timer;
    int16_t locked;
} reader;

typedef struct keypad {
    char code[16];
    uint32_t index;
    uint16_t timer;
    int16_t locked;
} code;

struct {
    struct reader readers[4];
    struct keypad keypads[4];
    repeating_timer_t timer;
    mutex_t guard;
} U2x = {
    .readers = {
        {.data = 0x00000000, .count = 0, .timer = 0, .locked = 0},
        {.data = 0x00000000, .count = 0, .timer = 0, .locked = 0},
        {.data = 0x00000000, .count = 0, .timer = 0, .locked = 0},
        {.data = 0x00000000, .count = 0, .timer = 0, .locked = 0},
    },
    .keypads = {
        {.code = {}, .index = 0, .timer = 0, .locked = 0},
        {.code = {}, .index = 0, .timer = 0, .locked = 0},
        {.code = {}, .index = 0, .timer = 0, .locked = 0},
        {.code = {}, .index = 0, .timer = 0, .locked = 0},
    }};

typedef struct keycode {
    char digit;
    uint32_t code4;
    uint32_t code8;
} keycode;

const keycode KEYCODES[] = {
    {'0', 0, 0x00f0},
    {'1', 1, 0x00e1},
    {'2', 2, 0x00d2},
    {'3', 3, 0x00c3},
    {'4', 4, 0x00b4},
    {'5', 5, 0x00a5},
    {'6', 6, 0x0096},
    {'7', 7, 0x0087},
    {'8', 8, 0x0078},
    {'9', 9, 0x0069},
    {'*', 10, 0x005a},
    {'#', 11, 0x004b},
};

const int KEYCODES_SIZE = sizeof(KEYCODES) / sizeof(keycode);

const PULLUP U2_PULLUPS[8] = {
    PULLUP_UP, // DI2
    PULLUP_UP, // DO2
    PULLUP_UP, // DI1
    PULLUP_UP, // DO1

    PULLUP_UP, // DO3
    PULLUP_UP, // DI3
    PULLUP_UP, // DO4
    PULLUP_UP, // DI4
};

void U2_setup() {
    infof("U2", "setup");

    // ... configure PCAL6408A
    int err;

    if ((err = PCAL6408A_init(U2)) != ERR_OK) {
        warnf("U2", "error initialising PCAL6408A (%d)", err);
    }

    if ((err = PCAL6408A_set_configuration(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error configuring PCAL6408A (%d)", err);
    }

    if ((err = PCAL6408A_set_polarity(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408A polarity (%d)", err);
    }

    if ((err = PCAL6408A_set_pullups(U2, U2_PULLUPS)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408A pullups (%d)", err);
    }

    if ((err = PCAL6408A_set_latched(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408A latches (%d)", err);
    }

    if ((err = PCAL6408A_set_interrupts(U2, 0x00)) != ERR_OK) {
        warnf("U2", "error enabling PCAL6408A interrupts (%d)", err);
    }

    // ... clear any existing interrupts
    uint8_t inputs;
    PCAL6408A_read(U2, &inputs);

    mutex_init(&U2x.guard);

    debugf("U2", "initial state %02x %08b", inputs, inputs);
}

// Enables interrupt handler
void U2_start() {
    infof("U2", "start");

    gpio_init(IOX_INT0);
    gpio_pull_up(IOX_INT0);
    gpio_add_raw_irq_handler(IOX_INT0, U2_on_interrupt);
    gpio_set_irq_enabled(IOX_INT0, GPIO_IRQ_LEVEL_LOW, true);

    add_repeating_timer_us(U2_TICK * 1000, U2_tick, NULL, &U2x.timer);
}

/*
 * Process  active reader/keypad timers.
 */
bool U2_tick(repeating_timer_t *rt) {
    uint32_t trace = trace_in(TRACE_U2_TICK);

    if (mutex_try_enter(&U2x.guard, NULL)) {
        struct reader *reader = U2x.readers;
        struct keypad *keypad = U2x.keypads;
        uint8_t door = 1;

        for (int i = 0; i < 4; i++) {
            U2_tick_reader(door, reader);
            U2_tick_keypad(door, keypad);

            reader++;
            keypad++;
            door++;
        }

        mutex_exit(&U2x.guard);
    }

    trace_out(TRACE_U2_TICK, trace);

    return true;
}

/*
 * Processes the Wiegand-26 interface.
 */
void U2_tick_reader(uint8_t door, struct reader *reader) {
    if (reader->locked > 0) {
        if (reader->locked > U2_TICK) {
            reader->locked -= U2_TICK;
        } else {
            reader->data = 0x00000000;
            reader->count = 0;
            reader->timer = 0;
            reader->locked = 0;
        }
    }

    // ... active?
    if (reader->count > 0) {
        reader->timer += U2_TICK;
    }

    // NTS: invalid input stream, so limit count to prevent it looping around.
    //      The invalid card/code will be processed if/when there is a 10ms interval
    //      in the data stream.
    if (reader->count > 64) {
        reader->count = 64;
    }

    if (reader->timer > U2_READ_TIMEOUT) {
        if (reader->count == 26) {
            U2_on_card_read(door, reader->data);
        } else if (reader->count == 4) {
            U2_on_keypad_digit(door, reader->data & 0x0000000f);
        } else if (reader->count == 8) {
            U2_on_keypad_digit(door, reader->data & 0x000000ff);
        } else {
            debugf("U2", "reader %d timeout (%d)", door, reader->count);
        }

        reader->data = 0x00000000;
        reader->count = 0;
        reader->timer = 0;
    }
}

/*
 * Process  active keypad timers.
 */
void U2_tick_keypad(uint8_t door, struct keypad *keypad) {
    if (keypad->locked > 0) {
        if (keypad->locked > U2_TICK) {
            keypad->locked -= U2_TICK;
        } else {
            keypad->index = 0;
            keypad->timer = 0;
            keypad->locked = 0;
        }
    }

    if (keypad->index > 0) {
        keypad->timer += U2_TICK;

        if (keypad->timer >= U2_CODE_TIMEOUT) {
            U2_on_keycode(door, keypad->code, keypad->index);
            keypad->index = 0;
            keypad->timer = 0;
        }
    }
}

void U2_on_interrupt() {
    if ((gpio_get_irq_event_mask(IOX_INT0) & GPIO_IRQ_LEVEL_LOW) != 0) {
        gpio_set_irq_enabled(IOX_INT0, GPIO_IRQ_LEVEL_LOW, false);

        I2C1_run(U2_read);

        // Ref. https://github.com/raspberrypi/pico-sdk/issues/108
        busy_wait_us_32(tRST);
        gpio_set_irq_enabled(IOX_INT0, GPIO_IRQ_LEVEL_LOW, true);
    }
}

void U2_read() {
    int err;
    uint8_t isr;
    uint8_t inputs;

    if ((err = PCAL6408A_isr(U2, &isr)) != ERR_OK) {
        warnf("U2", "error reading PCAL6408A ISR (%d)", err);
    }

    if ((err = PCAL6408A_read(U2, &inputs)) != ERR_OK) {
        warnf("U2", "error reading PCAL6408A inputs (%d)", err);
    } else {
        uint32_t v = inputs & isr;
        uint32_t msg = MSG_WIO | (v & 0x0fffffff);

        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "U2", "interrupt: queue full");
        }
    }
}

void U2_wio(uint8_t inputs) {
    mutex_enter_blocking(&U2x.guard);

    uint8_t door = 1;
    struct reader *reader = U2x.readers;

    for (int i = 0; i < 4; i++) {
        if ((inputs & D0[i]) != 0x00) {
            reader->data <<= 1;
            reader->data |= 0x00000000;
            reader->count++;
        }

        if ((inputs & D1[i]) != 0x00) {
            reader->data <<= 1;
            reader->data |= 0x00000001;
            reader->count++;
        }

        reader->timer = 0;

        reader++;
        door++;
    }

    mutex_exit(&U2x.guard);
}

void U2_on_card_read(uint8_t door, uint32_t v) {
    if (door >= 1 && door <= 4) {
        struct reader *reader = U2x.readers + (door - 1);

        if (reader->locked > 0) {
            debugf("U2", "READER %d  LOCKED", door);
        } else {
            int even = U2_bits(v & 0x03ffe000);
            int odd = U2_bits(v & 0x00001fff);
            uint32_t code = (v >> 1) & 0x00ffffff;

            bool ok = (even % 2) == 0 && (odd % 2) == 1;
            uint32_t facility_code = (code >> 16) & 0x000000ff;
            uint32_t card = code & 0x0000ffff;

            reader->locked = U2_CARD_LOCK;

            infof("U2", "READER %d  CARD %-3u%-5u %s", door, facility_code, card, ok ? "ok" : "error");
        }
    }
}

void U2_on_keypad_digit(uint8_t door, uint32_t v) {
    if (door >= 1 && door <= 4) {
        struct keypad *keypad = U2x.keypads + (door - 1);
        int keycode = v & 0x000000ff;

        for (int i = 0; i < KEYCODES_SIZE; i++) {
            if (KEYCODES[i].code4 == keycode || KEYCODES[i].code8 == keycode) {
                char digit = KEYCODES[i].digit;

                if (keypad->index < sizeof(keypad->code)) {
                    keypad->code[keypad->index++] = digit;

                    if (keypad->index >= sizeof(keypad->code)) {
                        U2_on_keycode(door, keypad->code, keypad->index);
                        keypad->index = 0;
                        keypad->timer = 0;
                    } else if (digit == '*' || digit == '#') {
                        U2_on_keycode(door, keypad->code, keypad->index);
                        keypad->index = 0;
                        keypad->timer = 0;
                    } else {
                        keypad->timer = 0;
                    }
                }

                break;
            }
        }
    }
}

void U2_on_keycode(uint8_t door, const char *code, int length) {
    if (length > 0 && door >= 1 && door <= 4) {
        struct keypad *keypad = U2x.keypads + (door - 1);

        if (keypad->locked > 0) {
            debugf("U2", "KEYPAD %d  LOCKED", door);
        } else {
            int N = length + 1;
            char *keycode;

            if ((keycode = calloc(N, 1)) != NULL) {
                snprintf(keycode, N, "%s", code);

                // uint32_t msg = MSG_CODE | ((uint32_t)s & 0x0fffffff); // SRAM_BASE is 0x20000000
                // if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                //     free(s);
                // }

                infof("U2", "KEYPAD %d  KEYCODE %s", door, keycode);
                free(keycode);
            }

            keypad->locked = U2_CODE_LOCK;
        }
    }
}

// Ref. https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
// Ref. https://stackoverflow.com/questions/109023/count-the-number-of-set-bits-in-a-32-bit-integer
int U2_bits(uint32_t v) {
    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    v = (v + (v >> 4)) & 0x0f0f0f0f;

    return (v * 0x01010101) >> 24;
}
