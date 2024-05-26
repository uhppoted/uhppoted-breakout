#include <stdio.h>

#include <pico/stdlib.h>

#include <RTC.h>
#include <RX8900SA.h>
#include <breakout.h>
#include <log.h>

/*
 * Initialises the RX8900SA.
 */
void RTC_init() {
    infof("RTC", "init");
}

/*
 * Resets the RTC.
 */
void RTC_reset() {
    infof("RTC", "reset");

    RX8900SA_reset(U5);
    RX8900SA_setup(U5);
}

void RTC_get_date(char yymmmdd[11]) {
    RX8900SA_get_date(U5, yymmmdd);
}

void RTC_set_date(uint16_t year, uint8_t month, uint8_t day) {
    RX8900SA_set_date(U5, year, month, day);
}

void RTC_get_time(char HHmmss[9]) {
    RX8900SA_get_time(U5, HHmmss);
}

void RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    RX8900SA_set_time(U5, hour, minute, second);
}
