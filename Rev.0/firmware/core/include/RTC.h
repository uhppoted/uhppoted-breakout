#pragma once

extern void RTC_init();
extern void RTC_reset();
extern void RTC_get_date(char date[11]);
extern void RTC_set_date(uint16_t year, uint8_t month, uint8_t day);
extern void RTC_get_time(char time[9]);
extern void RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second);
