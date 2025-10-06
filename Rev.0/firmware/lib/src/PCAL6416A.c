#include <PCAL6416A.h>

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
} PCAL6416A = {
    .INPUTS = 0x00,
    .OUTPUTS = 0x02,
    .POLARITY = 0x04,
    .CONFIGURATION = 0x06,
    .DRIVE = 0x40,
    .LATCH = 0x44,
    .PULLUPS = 0x46,
    .INTERRUPTS = 0x4a,
    .ISR = 0x4c,
    .OUTPUT_CONFIG = 0x4f,
};

const uint32_t PCAL6416A_DRIVE_1 = 0xffffffff;
const uint32_t PCAL6416A_DRIVE_0_75 = 0xaaaaaaaa;
const uint32_t PCAL6416A_DRIVE_0_5 = 0x55555555;
const uint32_t PCAL6416A_DRIVE_0_25 = 0x00000000;

int PCAL6416A_init(I2C dev) {
    return ERR_OK;
}

int PCAL6416A_get_configuration(I2C dev, uint16_t *configuration) {
    uint8_t buffer[] = {0, 0};
    int err;

    if ((err = I2C_read(dev, PCAL6416A.CONFIGURATION, &buffer[0])) != ERR_OK) {
        return err;
    } else if ((err = I2C_read(dev, PCAL6416A.CONFIGURATION + 1, &buffer[1])) != ERR_OK) {
        return err;
    } else {
        uint16_t hi = buffer[1];
        uint16_t lo = buffer[0];

        *configuration = ((hi << 8) & 0xff00) | ((lo << 0) & 0x00ff);

        return ERR_OK;
    }

    // if ((err = I2C_read_all(dev, PCAL6416A.CONFIGURATION, buffer, 2)) != ERR_OK) {
    //     return err;
    // } else {
    //     uint16_t hi = buffer[1];
    //     uint16_t lo = buffer[0];
    //
    //     *configuration = ((hi << 8) & 0xff00) | ((lo << 0) & 0x00ff);
    //
    //     return ERR_OK;
    // }
}

int PCAL6416A_set_configuration(I2C dev, uint16_t configuration) {
    uint8_t data[] = {
        (uint8_t)((configuration >> 0) & 0x00ff),
        (uint8_t)((configuration >> 8) & 0x00ff),
    };
    int err;

    if ((err = I2C_write(dev, PCAL6416A.CONFIGURATION, data[0])) != ERR_OK) {
        return err;
    } else if ((err = I2C_write(dev, PCAL6416A.CONFIGURATION + 1, data[1])) != ERR_OK) {
        return err;
    } else {
        return ERR_OK;
    }

    // uint8_t data[] = {
    //     (uint8_t)((configuration >> 0) & 0x00ff),
    //     (uint8_t)((configuration >> 8) & 0x00ff),
    // };
    //
    // return I2C_write_all(dev, PCAL6416A.CONFIGURATION, data, 2);
}

int PCAL6416A_set_polarity(I2C dev, uint16_t polarity) {
    uint8_t data[] = {
        (uint8_t)((polarity >> 0) & 0x00ff),
        (uint8_t)((polarity >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PCAL6416A.POLARITY, data, 2);
}

int PCAL6416A_set_latched(I2C dev, uint16_t latched) {
    uint8_t data[] = {
        (uint8_t)((latched >> 0) & 0x00ff),
        (uint8_t)((latched >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PCAL6416A.LATCH, data, 2);
}

int PCAL6416A_set_pullups(I2C dev, const PULLUP pullups[16]) {
    uint16_t enabled = 0x00;
    uint16_t up = 0x00;
    uint16_t mask = 0x0001;
    int err;

    for (int i = 0; i < 16; i++) {
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
        (uint8_t)((enabled >> 0) & 0x00ff),
        (uint8_t)((enabled >> 8) & 0x00ff),
        (uint8_t)((up >> 0) & 0x00ff),
        (uint8_t)((up >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PCAL6416A.PULLUPS, data, 4);
}

int PCAL6416A_set_open_drain(I2C dev, bool port0, bool port1) {
    uint8_t data = 0x00;

    data |= port0 ? 0x01 : 0x00;
    data |= port1 ? 0x02 : 0x00;

    return I2C_write(dev, PCAL6416A.OUTPUT_CONFIG, data);
}

int PCAL6416A_set_output_drive(I2C dev, const float drive[16]) {
    uint32_t mask = 0x00000003;
    uint32_t out = 0x00000000;

    for (int i = 0; i < 16; i++) {
        if (drive[i] <= 0.25f) {
            out |= PCAL6416A_DRIVE_0_25 & mask;
        } else if (drive[i] <= 0.5f) {
            out |= PCAL6416A_DRIVE_0_5 & mask;
        } else if (drive[i] <= 0.75f) {
            out |= PCAL6416A_DRIVE_0_75 & mask;
        } else {
            out |= PCAL6416A_DRIVE_1 & mask;
        }

        mask <<= 2;
    }

    uint8_t data[] = {
        (uint8_t)((out >> 0) & 0x000000ff),
        (uint8_t)((out >> 8) & 0x000000ff),
        (uint8_t)((out >> 16) & 0x000000ff),
        (uint8_t)((out >> 24) & 0x000000ff),
    };

    return I2C_write_all(dev, PCAL6416A.DRIVE, data, 4);
}

int PCAL6416A_set_interrupts(I2C dev, uint16_t interrupts) {
    uint8_t data[] = {
        (uint8_t)((interrupts >> 0) & 0x00ff),
        (uint8_t)((interrupts >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PCAL6416A.INTERRUPTS, data, 2);
}

int PCAL6416A_isr(I2C dev, uint16_t *data) {
    uint8_t buffer[] = {0, 0};
    int err;

    if ((err = I2C_read_all(dev, PCAL6416A.ISR, buffer, 2)) != ERR_OK) {
        return err;
    } else {
        uint16_t hi = ((uint16_t)buffer[1] << 8) & 0x00ff;
        uint16_t lo = ((uint16_t)buffer[0] << 0) & 0x00ff;

        *data = hi | lo;

        return ERR_OK;
    }
}

int PCAL6416A_read(I2C dev, uint16_t *data) {
    uint8_t buffer[] = {0, 0};
    int err;

    if ((err = I2C_read_all(dev, PCAL6416A.INPUTS, buffer, 2)) != ERR_OK) {
        return err;
    } else {
        uint16_t hi = ((uint16_t)buffer[1] << 8) & 0x00ff;
        uint16_t lo = ((uint16_t)buffer[0] << 0) & 0x00ff;

        *data = hi | lo;

        return ERR_OK;
    }
}

int PCAL6416A_write(I2C dev, uint16_t outputs) {
    uint8_t data[] = {
        (uint8_t)((outputs >> 0) & 0x00ff),
        (uint8_t)((outputs >> 8) & 0x00ff),
    };

    return I2C_write_all(dev, PCAL6416A.OUTPUTS, data, 2);
}

int PCAL6416A_readback(I2C dev, uint16_t *data) {
    uint8_t buffer[] = {0, 0};
    int err;

    if ((err = I2C_read_all(dev, PCAL6416A.OUTPUTS, buffer, 2)) != ERR_OK) {
        return err;
    } else {
        uint16_t hi = buffer[1];
        uint16_t lo = buffer[0];

        *data = ((hi << 8) & 0xff00) | ((lo << 0) & 0x00ff);

        return ERR_OK;
    }
}
