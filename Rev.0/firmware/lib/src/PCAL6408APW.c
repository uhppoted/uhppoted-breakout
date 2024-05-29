#include <PCAL6408APW.h>

#include <I2C0.h>
#include <I2C1.h>
#include <breakout.h>
#include <log.h>

const uint8_t INPUTS = 0x00;
const uint8_t OUTPUTS = 0x01;
const uint8_t POLARITY = 0x02;
const uint8_t CONFIGURATION = 0x03;
const uint8_t DRIVE = 0x40;
const uint8_t LATCH = 0x42;
const uint8_t PULLUPS = 0x43;
const uint8_t INTERRUPTS = 0x45;

int PCAL6408APW_init(uint8_t addr) {
    return ERR_OK;
}

int PCAL6408APW_set_configuration(uint8_t addr, uint8_t configuration) {
    if (addr == U2) {
        return I2C1_write(addr, CONFIGURATION, configuration);
    } else if (addr == U3) {
        return I2C0_write(addr, CONFIGURATION, configuration);
    } else {
        return ERR_UNKNOWN;
    }
}

int PCAL6408APW_set_polarity(uint8_t addr, uint8_t polarity) {
    if (addr == U2) {
        return I2C1_write(addr, POLARITY, polarity);
    } else if (addr == U3) {
        return I2C0_write(addr, POLARITY, polarity);
    } else {
        return ERR_UNKNOWN;
    }
}

int PCAL6408APW_set_latched(uint8_t addr, uint8_t latched) {
    if (addr == U2) {
        return I2C1_write(addr, LATCH, latched);
    } else if (addr == U3) {
        return I2C0_write(addr, LATCH, latched);
    } else {
        return ERR_UNKNOWN;
    }
}

int PCAL6408APW_set_pullups(uint8_t addr, uint8_t pullups) {
    uint8_t data[] = {
        pullups,
        pullups,
        0x00 // pulldowns
    };

    if (addr == U2) {
        return I2C1_write_all(addr, PULLUPS, data, 2);
    } else if (addr == U3) {
        return I2C0_write_all(addr, PULLUPS, data, 2);
    } else {
        return ERR_UNKNOWN;
    }
}

int PCAL6408APW_set_interrupts(uint8_t addr, uint8_t interrupts) {
    if (addr == U2) {
        return I2C1_write(addr, INTERRUPTS, interrupts);
    } else if (addr == U3) {
        return I2C0_write(addr, INTERRUPTS, interrupts);
    } else {
        return ERR_UNKNOWN;
    }
}

int PCAL6408APW_read(uint8_t addr, uint8_t *data) {
    if (addr == U2) {
        return I2C1_read(addr, INPUTS, data);
    } else if (addr == U3) {
        return I2C0_read(addr, INPUTS, data);
    } else {
        return ERR_UNKNOWN;
    }
}
