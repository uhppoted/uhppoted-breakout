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

const uint32_t PI4IOE5V6416_DRIVE_1 = 0xffffffff;
const uint32_t PI4IOE5V6416_DRIVE_0_75 = 0xaaaaaaaa;
const uint32_t PI4IOE5V6416_DRIVE_0_5 = 0x55555555;
const uint32_t PI4IOE5V6416_DRIVE_0_25 = 0x00000000;

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

int PI4IOE5V6416_set_open_drain(I2C dev, bool port0, bool port1) {
    uint8_t data = 0x00;

    data |= port0 ? 0x01 : 0x00;
    data |= port1 ? 0x02 : 0x00;

    return I2C_write(dev, PI4IOE5V6416.OUTPUT_CONFIG, data);
}

int PI4IOE5V6416_set_output_drive(I2C dev, const float drive[16]) {
    uint32_t mask = 0x00000003;
    uint32_t out = 0x00000000;

    for (int i = 0; i < 16; i++) {
        if (drive[i] <= 0.25f) {
            out |= PI4IOE5V6416_DRIVE_0_25 & mask;
        } else if (drive[i] <= 0.5f) {
            out |= PI4IOE5V6416_DRIVE_0_5 & mask;
        } else if (drive[i] <= 0.75f) {
            out |= PI4IOE5V6416_DRIVE_0_75 & mask;
        } else {
            out |= PI4IOE5V6416_DRIVE_1 & mask;
        }

        mask <<= 2;
    }

    uint8_t data[] = {
        (uint8_t)((out >> 0) & 0x000000ff),
        (uint8_t)((out >> 8) & 0x000000ff),
        (uint8_t)((out >> 16) & 0x000000ff),
        (uint8_t)((out >> 24) & 0x000000ff),
    };

    return I2C_write_all(dev, PI4IOE5V6416.DRIVE, data, 4);
}

int PI4IOE5V6416_read(I2C dev, uint16_t *data) {
    uint8_t buffer[] = {0, 0};
    int err;

    if ((err = I2C_read_all(dev, PI4IOE5V6416.INPUTS, buffer, 2)) != ERR_OK) {
        return err;
    } else {
        uint16_t hi = ((uint16_t)buffer[1] & 0x00ff) << 8;
        uint16_t lo = ((uint16_t)buffer[0] & 0x00ff) << 0;

        *data = hi | lo;

        return ERR_OK;
    }
}

int PI4IOE5V6416_write(I2C dev, uint16_t outputs) {
    uint8_t data[] = {
        (uint8_t)((outputs >> 0) & 0x00ff),
        (uint8_t)((outputs >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PI4IOE5V6416.OUTPUTS, data, 2);
}

int PI4IOE5V6416_readback(I2C dev, uint16_t *data) {
    uint8_t buffer[] = {0, 0};
    int err;

    if ((err = I2C_read_all(dev, PI4IOE5V6416.OUTPUTS, buffer, 2)) != ERR_OK) {
        return err;
    } else {
        uint16_t hi = buffer[1];
        uint16_t lo = buffer[0];

        *data = ((hi << 8) & 0xff00) | ((lo << 0) & 0x00ff);

        return ERR_OK;
    }
}
