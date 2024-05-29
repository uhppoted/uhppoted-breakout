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

int PCAL6408APW_init(I2C dev) {
    return ERR_OK;
}

int PCAL6408APW_set_configuration(I2C dev, uint8_t configuration) {
    return I2C_write(dev, CONFIGURATION, configuration);
}

int PCAL6408APW_set_polarity(I2C dev, uint8_t polarity) {
    return I2C_write(dev, POLARITY, polarity);
}

int PCAL6408APW_set_latched(I2C dev, uint8_t latched) {
    return I2C_write(dev, LATCH, latched);
}

int PCAL6408APW_set_pullups(I2C dev, uint8_t pullups) {
    uint8_t data[] = {
        pullups,
        pullups,
        0x00 // pulldowns
    };

    return I2C_write_all(dev, PULLUPS, data, 2);
}

int PCAL6408APW_set_interrupts(I2C dev, uint8_t interrupts) {
    return I2C_write(dev, INTERRUPTS, interrupts);
}

int PCAL6408APW_read(I2C dev, uint8_t *data) {
    return I2C_read(dev, INPUTS, data);
}
