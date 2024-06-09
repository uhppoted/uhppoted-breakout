#pragma once

#include <I2C.h>
#include <stdint.h>

extern int PCAL6408APW_init(I2C dev);
extern int PCAL6408APW_set_configuration(I2C dev, uint8_t polarity);
extern int PCAL6408APW_set_polarity(I2C dev, uint8_t polarity);
extern int PCAL6408APW_set_latched(I2C dev, uint8_t latched);
extern int PCAL6408APW_set_pullups(I2C dev, uint8_t pullups);
extern int PCAL6408APW_set_interrupts(I2C dev, uint8_t interrupts);
extern int PCAL6408APW_read(I2C dev, uint8_t *data);
extern int PCAL6408APW_isr(I2C dev, uint8_t *data);
