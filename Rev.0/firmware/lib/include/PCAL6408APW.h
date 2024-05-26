#pragma once

#include <stdint.h>

extern int PCAL6408APW_init(uint8_t addr);
extern int PCAL6408APW_set_configuration(uint8_t addr, uint8_t polarity);
extern int PCAL6408APW_set_polarity(uint8_t addr, uint8_t polarity);
extern int PCAL6408APW_set_latched(uint8_t addr, uint8_t latched);
extern int PCAL6408APW_set_pullups(uint8_t addr, uint8_t pullups);
extern int PCAL6408APW_read(uint8_t addr, uint8_t *data);
