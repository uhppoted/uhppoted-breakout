#include <PI4IOE5V6416.h>

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
    uint8_t OUTPUT_CONFIG;
} PI4IOE5V6416 = {
    .INPUTS = 0x00,
    .OUTPUTS = 0x02,
    .POLARITY = 0x04,
    .CONFIGURATION = 0x06,
    .DRIVE = 0x40,
    .LATCH = 0x44,
    .PULLUPS = 0x46,
    .INTERRUPTS = 0x4a,
    .OUTPUT_CONFIG = 0x4f,
};

int PI4IOE5V6416_init(I2C dev) {
    return ERR_OK;
}

int PI4IOE5V6416_set_configuration(I2C dev, uint16_t configuration) {
    uint8_t data[] = {
        (uint8_t)((configuration >> 0) & 0x00ff),
        (uint8_t)((configuration >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PI4IOE5V6416.CONFIGURATION, data, 2);
}

int PI4IOE5V6416_set_polarity(I2C dev, uint16_t polarity) {
    uint8_t data[] = {
        (uint8_t)((polarity >> 0) & 0x00ff),
        (uint8_t)((polarity >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PI4IOE5V6416.POLARITY, data, 2);
}

int PI4IOE5V6416_set_latched(I2C dev, uint16_t latched) {
    uint8_t data[] = {
        (uint8_t)((latched >> 0) & 0x00ff),
        (uint8_t)((latched >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PI4IOE5V6416.LATCH, data, 2);
}

int PI4IOE5V6416_set_pullups(I2C dev, uint16_t pullups) {
    uint8_t data[] = {
        (uint8_t)((pullups >> 0) & 0x00ff),
        (uint8_t)((pullups >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PI4IOE5V6416.PULLUPS, data, 2);
}

int PI4IOE5V6416_write(I2C dev, uint16_t outputs) {
    uint8_t data[] = {
        (uint8_t)((outputs >> 0) & 0x00ff),
        (uint8_t)((outputs >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PI4IOE5V6416.OUTPUTS, data, 2);
}
