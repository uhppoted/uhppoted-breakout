#pragma once

#include <I2C.h>
#include <stdint.h>

#include "lib.h"

extern int PI4IOE5V6416_init(I2C dev);
extern int PI4IOE5V6416_set_configuration(I2C dev, uint16_t configuration);
extern int PI4IOE5V6416_set_polarity(I2C dev, uint16_t polarity);
extern int PI4IOE5V6416_set_latched(I2C dev, uint16_t latched);
extern int PI4IOE5V6416_set_pullups(I2C dev, const PULLUP pullups[16]);
extern int PI4IOE5V6416_set_open_drain(I2C dev, bool port0, bool port1);
extern int PI4IOE5V6416_set_output_drive(I2C dev, const float drive[16]);
// extern int PI4IOE5V6416_read(I2C dev, uint16_t *data);
extern int PI4IOE5V6416_write(I2C dev, uint16_t data);
extern int PI4IOE5V6416_readback(I2C dev, uint16_t *data);
