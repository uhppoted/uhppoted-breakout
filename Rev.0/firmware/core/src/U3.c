#include <stdio.h>

#include <pico/stdlib.h>

#include <PCAL6408APW.h>
#include <U3.h>
#include <breakout.h>
#include <log.h>

void U3_on_interrupt(void);

void U3_init() {
    infof("U3", "init");

    // ... configure PCAL6408APW
    int err;

    if ((err = PCAL6408APW_init(U3)) != ERR_OK) {
        warnf("U3", "error initialising PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_configuration(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error configuring PCAL6408APW (%d)", err);
    }

    if ((err = PCAL6408APW_set_polarity(U3, 0x00)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408APW polarity (%d)", err);
    }

    if ((err = PCAL6408APW_set_latched(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408APW latches (%d)", err);
    }

    if ((err = PCAL6408APW_set_pullups(U3, 0xff)) != ERR_OK) {
        warnf("U3", "error setting PCAL6408APW pullups (%d)", err);
    }

    if ((err = PCAL6408APW_set_interrupts(U3, 0x00)) != ERR_OK) {
        warnf("U3", "error enabling PCAL6408APW interrupts (%d)", err);
    }

    uint8_t inputs;
    PCAL6408APW_read(U3, &inputs); // clear any existing interrupts

    debugf("U3", "initial state %02x %08b", inputs, inputs);

    // ... attach interrupt handler
    gpio_init(IOX_INT1);
    gpio_pull_up(IOX_INT1);
    gpio_add_raw_irq_handler(IOX_INT1, U3_on_interrupt);
    gpio_set_irq_enabled(IOX_INT1, GPIO_IRQ_LEVEL_LOW, true);
}

void U3_on_interrupt(void) {
    uint32_t mask = GPIO_IRQ_LEVEL_LOW | GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
    uint8_t inputs;
    int err;

    if (gpio_get_irq_event_mask(IOX_INT1) & mask) {
        gpio_acknowledge_irq(IOX_INT1, mask);

        if ((err = PCAL6408APW_read(U3, &inputs)) != ERR_OK) {
            warnf("U3", "error reading PCAL6408APW inputs (%d)", err);
        }

        uint32_t msg = MSG_INPUTS | ((uint32_t)inputs & 0x0fffffff);

        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            warnf("U3", "discarded  %02x %08b", inputs, inputs);
        }
    }
}

// void U3_debug() {
//     infof("U3", "debug");
//
//     for (int i = 0; i < 30; i++) {
//         int int1 = gpio_get(IOX_INT1);
//         uint8_t inputs;
//         int err;

//         if ((err = PCAL6408APW_read(U3, &inputs)) != ERR_OK) {
//             warnf("U3", "error reading PCAL6408APW inputs (%d)", err);
//         }

//         infof("U3", "INPUTS %02x %08b  INT1:%d", inputs, inputs, int1);
//         sleep_ms(1000);
//     }
// }
