#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C0.h>
#include <RTC.h>
#include <RX8900SA.h>
#include <breakout.h>
#include <log.h>
#include <state.h>

void RTC_get(void *data);
bool RTC_on_update(repeating_timer_t *rt);
uint8_t dow(uint16_t year, uint8_t month, uint8_t day);

struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dow;
    repeating_timer_t timer;
    struct closure task;
    mutex_t guard;
} RTC = {
    .year = 2000,
    .month = 12,
    .day = 31,
    .hour = 0,
    .minute = 0,
    .second = 0,
    .dow = 1,
    .task = {
        .f = &RTC_get,
        .data = &RTC,
    },
};

/*
 * Initialises the RX8900SA.
 */
void RTC_init() {
    infof("RTC", "init");
    RX8900SA_init(U5);

    mutex_init(&RTC.guard);

    add_repeating_timer_ms(1000, RTC_on_update, NULL, &RTC.timer);

    infof("RTC", "initialised %p", &RTC);
}

bool RTC_on_update(repeating_timer_t *rt) {
    if (!I2C0_push(&RTC.task)) {
        set_error(ERR_QUEUE_FULL);
    }

    return true;
}

void RTC_get(void *data) {
    uint16_t year = 0;
    uint8_t month = 0;
    uint8_t day = 0;
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
    uint8_t dow = 0;
    int err;

    if (mutex_try_enter(&RTC.guard, NULL)) {
        if ((err = RX8900SA_get_date(U5, &year, &month, &day)) != ERR_OK) {
            //     warnf("RTC", "get-date error %d", err);
            set_error(ERR_RX8900SA);
        } else {
            RTC.year = year;
            RTC.month = month;
            RTC.day = day;
        }

        if ((err = RX8900SA_get_time(U5, &hour, &minute, &second)) != ERR_OK) {
            // warnf("RTC", "get-time error %d", err);
            set_error(ERR_RX8900SA);
        } else {
            RTC.hour = hour;
            RTC.minute = minute;
            RTC.second = second;
        }

        if ((err = RX8900SA_get_dow(U5, &dow)) != ERR_OK) {
            // warnf("RTC", "get-dow error %d", err);
            set_error(ERR_RX8900SA);
        } else {
            RTC.dow = dow;
        }

        mutex_exit(&RTC.guard);
    }
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
    mutex_enter_blocking(&RTC.guard);
    uint16_t year = RTC.year;
    uint8_t month = RTC.month;
    uint8_t day = RTC.day;
    mutex_exit(&RTC.guard);

    snprintf(yymmmdd, N, "%04u-%02u-%02u", year, month, day);
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
    mutex_enter_blocking(&RTC.guard);
    uint8_t hour = RTC.hour;
    uint8_t minute = RTC.minute;
    uint8_t second = RTC.second;
    mutex_exit(&RTC.guard);

    snprintf(HHmmss, N, "%02u:%02u:%02u", hour, minute, second);
}

void RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    int err;

    if ((err = RX8900SA_set_time(U5, hour, minute, second)) != ERR_OK) {
        warnf("RTC", "set-time error %d", err);
    }
}

void RTC_get_dow(char *weekday, int N) {
    mutex_enter_blocking(&RTC.guard);
    uint8_t dow = RTC.dow;
    mutex_exit(&RTC.guard);

    if (RTC.dow == SUNDAY) {
        snprintf(weekday, N, "Sunday");
    } else if (RTC.dow == MONDAY) {
        snprintf(weekday, N, "Monday");
    } else if (RTC.dow == TUESDAY) {
        snprintf(weekday, N, "Tuesday");
    } else if (RTC.dow == WEDNESDAY) {
        snprintf(weekday, N, "Wednesday");
    } else if (RTC.dow == THURSDAY) {
        snprintf(weekday, N, "Thursday");
    } else if (RTC.dow == FRIDAY) {
        snprintf(weekday, N, "Friday");
    } else if (RTC.dow == SATURDAY) {
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