#include <stdio.h>

#include <pico/stdlib.h>

#include <RTC.h>
#include <RX8900SA.h>
#include <breakout.h>
#include <log.h>

uint8_t dow(uint16_t year, uint8_t month, uint8_t day);

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

void RTC_get_date(char *yymmmdd, int N) {
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    int err;

    if ((err = RX8900SA_get_date(U5, &year, &month, &day)) != ERR_OK) {
        warnf("RTC", "get-date error %d", err);

        snprintf(yymmmdd, N, "2000-01-01");
    } else {
        snprintf(yymmmdd, N, "%04u-%02u-%02u", year, month, day);
    }
}

void RTC_set_date(uint16_t year, uint8_t month, uint8_t day) {
    uint8_t weekday = dow(year, month, day);
    int err;

    if ((err = RX8900SA_set_date(U5, year, month, day)) != ERR_OK) {
        warnf("RTC", "set-date error %d", err);
    } else if ((err = RX8900SA_set_dow(U5, weekday)) != ERR_OK) {
        warnf("RTC", "set-date error %d", err);
    }
}

void RTC_get_time(char *HHmmss, int N) {
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
    int err;

    if ((err = RX8900SA_get_time(U5, &hour, &minute, &second)) != ERR_OK) {
        warnf("RTC", "get-time error %d", err);

        snprintf(HHmmss, N, "00:00:00");
    } else {
        snprintf(HHmmss, N, "%02u:%02u:%02u", hour, minute, second);
    }
}

void RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    int err;

    if ((err = RX8900SA_set_time(U5, hour, minute, second)) != ERR_OK) {
        warnf("RTC", "set-time error %d", err);
    }
}

void RTC_get_dow(char *weekday, int N) {
    uint8_t dow = 0;
    int err;

    if ((err = RX8900SA_get_dow(U5, &dow)) != ERR_OK) {
        warnf("RTC", "get-dow error %d", err);
    }

    if (dow == SUNDAY) {
        snprintf(weekday, N, "Sunday");
    } else if (dow == MONDAY) {
        snprintf(weekday, N, "Monday");
    } else if (dow == TUESDAY) {
        snprintf(weekday, N, "Tuesday");
    } else if (dow == WEDNESDAY) {
        snprintf(weekday, N, "Wednesday");
    } else if (dow == THURSDAY) {
        snprintf(weekday, N, "Thursday");
    } else if (dow == FRIDAY) {
        snprintf(weekday, N, "Friday");
    } else if (dow == SATURDAY) {
        snprintf(weekday, N, "Saturday");
    } else {
        snprintf(weekday, N, "???");
    }
}

// Tøndering's variation of Zeller's congruence
uint8_t dow(uint16_t year, uint8_t month, uint8_t day) {
    int K = year % 100;
    int J = year / 100;
    int Y = ((month == 1) || (month == 2)) ? K - 1 : K;
    int m = ((month == 1) || (month == 2)) ? month + 12 : month;
    int q = day;

    int h = (q + (31 * (m - 2) / 12) + Y + (Y / 4) - (Y / 100) + (Y / 400)) % 7;

    switch (h) {
    case 0:
        return SUNDAY;
    case 1:
        return MONDAY;
    case 2:
        return TUESDAY;
    case 3:
        return WEDNESDAY;
    case 4:
        return THURSDAY;
    case 5:
        return FRIDAY;
    case 6:
        return SATURDAY;
    default:
        return SUNDAY;
    }
}