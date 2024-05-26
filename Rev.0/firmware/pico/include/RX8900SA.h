#pragma once

#include <stdint.h>

extern void RX8900SA_init(uint8_t addr);
extern void RX8900SA_reset(uint8_t addr);
extern void RX8900SA_setup(uint8_t addr);
extern void RX8900SA_get_date(uint8_t addr, char date[11]);
extern void RX8900SA_set_date(uint8_t addr, uint16_t year, uint8_t month, uint8_t day);
extern void RX8900SA_get_time(uint8_t addr, char time[9]);
extern void RX8900SA_set_time(uint8_t addr, uint8_t hour, uint8_t minute, uint8_t second);
