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

    RX8900SA_init(U5);
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
    int err;

    if ((err = RX8900SA_get_date(U5, yymmmdd)) != ERR_OK) {
        warnf("RTC", "get-date error %d", err);

        snprintf(yymmmdd, 11, "2000-01-01");
    }
}

void RTC_set_date(uint16_t year, uint8_t month, uint8_t day) {
    int err;

    if ((err = RX8900SA_set_date(U5, year, month, day)) != ERR_OK) {
        warnf("RTC", "set-date error %d", err);
    }
}

void RTC_get_time(char HHmmss[9]) {
    int err;

    if ((err = RX8900SA_get_time(U5, HHmmss)) != ERR_OK) {
        warnf("RTC", "get-time error %d", err);

        snprintf(HHmmss, 9, "00:00:00");
    }
}

void RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    int err;

    if ((err = RX8900SA_set_time(U5, hour, minute, second)) != ERR_OK) {
        warnf("RTC", "set-time error %d", err);
    }
}
