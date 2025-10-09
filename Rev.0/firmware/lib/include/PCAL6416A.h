#pragma once

#include <I2C.h>
#include <stdint.h>

#include "lib.h"

extern int PCAL6416A_init(I2C dev);

extern int PCAL6416A_get_configuration(I2C dev, uint16_t *configuration);
extern int PCAL6416A_set_configuration(I2C dev, uint16_t configuration);

extern int PCAL6416A_get_polarity(I2C dev, uint16_t *polarity);
extern int PCAL6416A_set_polarity(I2C dev, uint16_t polarity);

extern int PCAL6416A_get_latched(I2C dev, uint16_t *latched);
extern int PCAL6416A_set_latched(I2C dev, uint16_t latched);

extern int PCAL6416A_get_pullups(I2C dev, uint32_t *pullups);
extern int PCAL6416A_set_pullups(I2C dev, const PULLUP pullups[16]);

extern int PCAL6416A_get_open_drain(I2C dev, uint8_t *byte);
extern int PCAL6416A_set_open_drain(I2C dev, bool port0, bool port1);

extern int PCAL6416A_get_output_drive(I2C dev, uint32_t *drive);
extern int PCAL6416A_set_output_drive(I2C dev, const float drive[16]);

extern int PCAL6416A_get_interrupts(I2C dev, uint16_t *interrupts);
extern int PCAL6416A_set_interrupts(I2C dev, uint16_t interrupts);

extern int PCAL6416A_isr(I2C dev, uint16_t *data);
extern int PCAL6416A_read(I2C dev, uint16_t *data);
extern int PCAL6416A_write(I2C dev, uint16_t data);
extern int PCAL6416A_readback(I2C dev, uint16_t *data);
