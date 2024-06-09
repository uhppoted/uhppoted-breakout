#include <PCAL6408APW.h>

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
} PCAL6408APW = {
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

int PCAL6408APW_init(I2C dev) {
    return ERR_OK;
}

int PCAL6408APW_set_configuration(I2C dev, uint8_t configuration) {
    return I2C_write(dev, PCAL6408APW.CONFIGURATION, configuration);
}

int PCAL6408APW_set_polarity(I2C dev, uint8_t polarity) {
    return I2C_write(dev, PCAL6408APW.POLARITY, polarity);
}

int PCAL6408APW_set_latched(I2C dev, uint8_t latched) {
    return I2C_write(dev, PCAL6408APW.LATCH, latched);
}

int PCAL6408APW_set_pullups(I2C dev, uint8_t pullups) {
    uint8_t data[] = {
        pullups,
        pullups,
    };

    return I2C_write_all(dev, PCAL6408APW.PULLUPS, data, 2);
}

int PCAL6408APW_set_interrupts(I2C dev, uint8_t interrupts) {
    return I2C_write(dev, PCAL6408APW.INTERRUPTS, interrupts);
}

int PCAL6408APW_read(I2C dev, uint8_t *data) {
    return I2C_read(dev, PCAL6408APW.INPUTS, data);
}

int PCAL6408APW_isr(I2C dev, uint8_t *data) {
    return I2C_read(dev, PCAL6408APW.ISR, data);
}
