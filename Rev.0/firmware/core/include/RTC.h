#pragma once

#include <stdbool.h>

extern void RTC_init();
extern void RTC_start();
extern void RTC_reset();
extern void RTC_get_date(char *date, int N);
extern bool RTC_set_date(uint16_t year, uint8_t month, uint8_t day);
extern void RTC_get_time(char *time, int N);
extern bool RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second);
extern void RTC_get_datetime(char *datetime, int N);
extern bool RTC_set_datetime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
extern void RTC_get_dow(char *weekday, int N);
