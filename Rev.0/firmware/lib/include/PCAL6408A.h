#pragma once

#include <I2C.h>
#include <stdint.h>

#include "lib.h"

extern int PCAL6408A_init(I2C dev);
extern int PCAL6408A_set_configuration(I2C dev, uint8_t polarity);
extern int PCAL6408A_set_polarity(I2C dev, uint8_t polarity);
extern int PCAL6408A_set_latched(I2C dev, uint8_t latched);
extern int PCAL6408A_set_pullups(I2C dev, const PULLUP pullups[8]);
extern int PCAL6408A_set_open_drain(I2C dev, bool port0);
extern int PCAL6408A_set_output_drive(I2C dev, const float drive[8]);
extern int PCAL6408A_set_interrupts(I2C dev, uint8_t interrupts);
extern int PCAL6408A_isr(I2C dev, uint8_t *data);
extern int PCAL6408A_read(I2C dev, uint8_t *data);
extern int PCAL6408A_write(I2C dev, uint8_t data);
extern int PCAL6408A_readback(I2C dev, uint8_t *data);
