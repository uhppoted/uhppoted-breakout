#pragma once

#include <I2C.h>
#include <stdint.h>

extern int PI4IOE5V6416_init(I2C dev);
extern int PI4IOE5V6416_set_configuration(I2C dev, uint16_t configuration);
extern int PI4IOE5V6416_set_polarity(I2C dev, uint16_t polarity);
extern int PI4IOE5V6416_set_latched(I2C dev, uint16_t latched);
extern int PI4IOE5V6416_set_pullups(I2C dev, uint16_t pullups);
extern int PI4IOE5V6416_write(I2C dev, uint16_t data);