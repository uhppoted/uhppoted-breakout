#include <stdio.h>

#include <pico/stdlib.h>

#include <PCAL6408APW.h>
#include <U2.h>
#include <breakout.h>
#include <log.h>

void U2_on_interrupt(void);

const uint8_t tRST = 1; // tRST (µs) for PCAL6408APW_interrupt line

void U2_init() {
    infof("U2", "init");

    // ... configure PCAL6408APW
    int err;

    if ((err = PCAL6408APW_init(U2)) != ERR_OK) {
        warnf("U2", "error initialising PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_configuration(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error configuring PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_polarity(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408APW polarity (%d)", err);
    }

    if ((err = PCAL6408APW_set_latched(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408APW latches (%d)", err);
    }

    if ((err = PCAL6408APW_set_pullups(U2, 0xff)) != ERR_OK) {
        warnf("U2", "error setting PCAL6408APW pullups (%d)", err);
    }

    if ((err = PCAL6408APW_set_interrupts(U2, 0x00)) != ERR_OK) {
        warnf("U2", "error enabling PCAL6408APW interrupts (%d)", err);
    }

    uint8_t inputs;
    PCAL6408APW_read(U2, &inputs); // clear any existing interrupts

    debugf("U2", "initial state %02x %08b", inputs, inputs);

    // ... attach interrupt handler
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

        if ((err = PCAL6408APW_isr(U2, &isr)) != ERR_OK) {
            warnf("U2", "error reading PCAL6408APW ISR (%d)", err);
        }

        if ((err = PCAL6408APW_read(U2, &inputs)) != ERR_OK) {
            warnf("U2", "error reading PCAL6408APW inputs (%d)", err);
        } else {
            uint32_t p = isr;
            uint32_t q = inputs;
            uint32_t v = (p << 8u) | q;

            uint32_t msg = MSG_WIO | (v & 0x0fffffff);

            if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                warnf("U2", "discarded  INPUTS:%02x (%08b) ISR:%02x (%08b)", inputs, inputs, isr, isr);
            }
        }

        // Ref. https://github.com/raspberrypi/pico-sdk/issues/108
        busy_wait_us_32(tRST);
        gpio_set_irq_enabled(IOX_INT0, GPIO_IRQ_LEVEL_LOW, true);
    }
}

// void U2_debug() {
//     infof("U2", "debug");
//
//     for (int i = 0; i < 30; i++) {
//         int int0 = gpio_get(IOX_INT0);
//         uint8_t inputs;
//         int err;
//
//         if ((err = PCAL6408APW_read(U2, &inputs)) != ERR_OK) {
//             warnf("U2", "error reading PCAL6408APW inputs (%d)", err);
//         }
//
//         infof("U2", "INPUTS %02x %08b  INT0:%d", inputs, inputs, int0);
//         sleep_ms(1000);
//     }
// }
