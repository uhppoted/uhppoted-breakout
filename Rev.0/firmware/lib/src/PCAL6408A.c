#include <PCAL6408A.h>

#include <breakout.h>
#include <log.h>

const struct {
    uint8_t INPUTS;
    uint8_t OUTPUTS;
    uint8_t POLARITY;
    uint8_t CONFIGURATION;
    uint8_t DRIVE;
    uint8_t LATCH;
    uint8_t PULLUPS;
    uint8_t INTERRUPTS;
    uint8_t ISR;
    uint8_t OUTPUT_CONFIG;
} PCAL6408A = {
    .INPUTS = 0x00,
    .OUTPUTS = 0x01,
    .POLARITY = 0x02,
    .CONFIGURATION = 0x03,
    .DRIVE = 0x40,
    .LATCH = 0x42,
    .PULLUPS = 0x43,
    .INTERRUPTS = 0x45,
    .ISR = 0x46,
    .OUTPUT_CONFIG = 0x4f,
};

const uint16_t PCAL6408A_DRIVE_1 = 0xffff;
const uint16_t PCAL6408A_DRIVE_0_75 = 0xaaaa;
const uint16_t PCAL6408A_DRIVE_0_5 = 0x5555;
const uint16_t PCAL6408A_DRIVE_0_25 = 0x0000;

int PCAL6408A_init(I2C dev) {
    return ERR_OK;
}

int PCAL6408A_set_configuration(I2C dev, uint8_t configuration) {
    return I2C_write(dev, PCAL6408A.CONFIGURATION, configuration);
}

int PCAL6408A_set_polarity(I2C dev, uint8_t polarity) {
    return I2C_write(dev, PCAL6408A.POLARITY, polarity);
}

int PCAL6408A_set_latched(I2C dev, uint8_t latched) {
    return I2C_write(dev, PCAL6408A.LATCH, latched);
}

int PCAL6408A_set_pullups(I2C dev, const PULLUP pullups[8]) {
    uint8_t enabled = 0x00;
    uint8_t up = 0x00;
    uint8_t mask = 0x0001;
    int err;

    for (int i = 0; i < 8; i++) {
        if (pullups[i] == PULLUP_UP) {
            enabled |= mask;
            up |= mask;
        } else if (pullups[i] == PULLUP_DOWN) {
            enabled |= mask;
        }

        mask <<= 1;
    }

    // ... enable/disable internal pullups
    uint8_t data[] = {
        enabled,
        up,
    };

    return I2C_write_all(dev, PCAL6408A.PULLUPS, data, 2);
}

int PCAL6408A_set_open_drain(I2C dev, bool port0) {
    uint8_t data = port0 ? 0x01 : 0x00;

    return I2C_write(dev, PCAL6408A.OUTPUT_CONFIG, data);
}

int PCAL6408A_set_output_drive(I2C dev, const float drive[8]) {
    uint16_t mask = 0x0003;
    uint16_t out = 0x0000;

    for (int i = 0; i < 8; i++) {
        if (drive[i] <= 0.25f) {
            out |= PCAL6408A_DRIVE_0_25 & mask;
        } else if (drive[i] <= 0.5f) {
            out |= PCAL6408A_DRIVE_0_5 & mask;
        } else if (drive[i] <= 0.75f) {
            out |= PCAL6408A_DRIVE_0_75 & mask;
        } else {
            out |= PCAL6408A_DRIVE_1 & mask;
        }

        mask <<= 2;
    }

    uint8_t data[] = {
        (uint8_t)((out >> 0) & 0x000000ff),
        (uint8_t)((out >> 8) & 0x000000ff),
    };

    return I2C_write_all(dev, PCAL6408A.DRIVE, data, 2);
}

int PCAL6408A_set_interrupts(I2C dev, uint8_t interrupts) {
    return I2C_write(dev, PCAL6408A.INTERRUPTS, interrupts);
}

int PCAL6408A_isr(I2C dev, uint8_t *data) {
    return I2C_read(dev, PCAL6408A.ISR, data);
}

int PCAL6408A_read(I2C dev, uint8_t *data) {
    return I2C_read(dev, PCAL6408A.INPUTS, data);
}

int PCAL6408A_write(I2C dev, uint8_t outputs) {
    return I2C_write(dev, PCAL6408A.OUTPUTS, outputs);
}

int PCAL6408A_readback(I2C dev, uint8_t *data) {
    return I2C_read(dev, PCAL6408A.OUTPUTS, data);
}
