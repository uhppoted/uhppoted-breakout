#pragma once

#include <stdint.h>

extern int RX8900SA_init(uint8_t addr);
extern int RX8900SA_reset(uint8_t addr);
extern int RX8900SA_setup(uint8_t addr);
extern int RX8900SA_get_date(uint8_t addr, uint16_t *year, uint8_t *month, uint8_t *day);
extern int RX8900SA_set_date(uint8_t addr, uint16_t year, uint8_t month, uint8_t day);
extern int RX8900SA_get_time(uint8_t addr, uint8_t *hour, uint8_t *minute, uint8_t *second);
extern int RX8900SA_set_time(uint8_t addr, uint8_t hour, uint8_t minute, uint8_t second);
extern int RX8900SA_get_dow(uint8_t addr, uint8_t *weekday);
extern int RX8900SA_set_dow(uint8_t addr, uint8_t weekday);

extern const uint8_t SUNDAY;
extern const uint8_t MONDAY;
extern const uint8_t TUESDAY;
extern const uint8_t WEDNESDAY;
extern const uint8_t THURSDAY;
extern const uint8_t FRIDAY;
extern const uint8_t SATURDAY;
