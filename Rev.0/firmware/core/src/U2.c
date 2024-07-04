#include <stdio.h>

#include <pico/stdlib.h>

#include <PCAL6408A.h>
#include <U2.h>
#include <breakout.h>
#include <log.h>
#include <state.h>

void U2_on_interrupt(void);
int bits(uint32_t v);

const uint8_t tRST = 1; // tRST (µs) for PCAL6408A interrupt line
const uint16_t DO1 = 0x08;
const uint16_t DI1 = 0x04;

struct {
    uint32_t reader;
    uint8_t count;

} U2x = {
    .reader = 0x00000000,
    .count = 0,
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

    if ((err = PCAL6408A_set_pullups(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408A pullups (%d)", err);
    }

    if ((err = PCAL6408A_set_latched(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408A latches (%d)", err);
    }

    if ((err = PCAL6408A_set_interrupts(U2, 0x00)) != ERR_OK) {
        warnf("U2", "error enabling PCAL6408A interrupts (%d)", err);
    }

    uint8_t inputs;
    PCAL6408A_read(U2, &inputs); // clear any existing interrupts

    debugf("U2", "initial state %02x %08b", inputs, inputs);
}

// Enables interrupt handler
void U2_start() {
    infof("U2", "start");

    gpio_init(IOX_INT0);
    gpio_pull_up(IOX_INT0);
    gpio_add_raw_irq_handler(IOX_INT0, U2_on_interrupt);
    gpio_set_irq_enabled(IOX_INT0, GPIO_IRQ_LEVEL_LOW, true);
}

void U2_on_interrupt(void) {
    uint8_t isr;
    uint8_t inputs;
    int err;

    if ((gpio_get_irq_event_mask(IOX_INT0) & GPIO_IRQ_LEVEL_LOW) != 0) {
        gpio_set_irq_enabled(IOX_INT0, GPIO_IRQ_LEVEL_LOW, false);

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

        // Ref. https://github.com/raspberrypi/pico-sdk/issues/108
        busy_wait_us_32(tRST);
        gpio_set_irq_enabled(IOX_INT0, GPIO_IRQ_LEVEL_LOW, true);
    }
}

void U2_wio(uint8_t inputs) {
    // debugf("U2", "inputs:%02x (%08b)", inputs, inputs);

    if ((inputs & DO1) != 0x00) {
        U2x.reader <<= 1;
        U2x.reader |= 0x00000001;
        U2x.count++;
    }

    if ((inputs & DI1) != 0x00) {
        U2x.reader <<= 1;
        U2x.reader |= 0x00000000;
        U2x.count++;
    }

    // debugf("U2", "R1 %-2d %032b", U2x.count, U2x.reader);

    if (U2x.count == 26) {
        U2_on_card_read(U2x.reader);
        U2x.reader = 0x00000000;
        U2x.count = 0x00000000;
    }
}

void U2_on_card_read(uint32_t v) {
    int even = bits(v & 0x03ffe000);
    int odd = bits(v & 0x00001fff);
    uint32_t code = (v >> 1) & 0x00ffffff;

    bool ok = (even % 2) == 0 && (odd % 2) == 1;
    uint32_t facility_code = (code >> 16) & 0x000000ff;
    uint32_t card = code & 0x0000ffff;

    infof("U2", "CARD %-3u%-5u %s", facility_code, card, ok ? "ok" : "error");
}

// Ref. https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
// Ref. https://stackoverflow.com/questions/109023/count-the-number-of-set-bits-in-a-32-bit-integer
int bits(uint32_t v) {
    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    v = (v + (v >> 4)) & 0x0f0f0f0f;

    return (v * 0x01010101) >> 24;
}
