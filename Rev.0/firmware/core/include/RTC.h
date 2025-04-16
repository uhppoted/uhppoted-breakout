#pragma once

#include <stdbool.h>

extern void RTC_init();
extern void RTC_start();
extern void RTC_reset();
extern bool RTC_ready();

extern bool RTC_get_datetime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *dow);
extern bool RTC_set_datetime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
extern bool RTC_set_date(uint16_t year, uint8_t month, uint8_t day);
extern bool RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second);

extern const uint8_t SUNDAY;
extern const uint8_t MONDAY;
extern const uint8_t TUESDAY;
extern const uint8_t WEDNESDAY;
extern const uint8_t THURSDAY;
extern const uint8_t FRIDAY;
extern const uint8_t SATURDAY;
