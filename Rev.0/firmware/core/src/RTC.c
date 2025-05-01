#include <stdio.h>
#include <stdlib.h>

#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C0.h>
#include <RTC.h>
#include <RX8900SA.h>
#include <breakout.h>
#include <log.h>
#include <mempool.h>
#include <types/datetime.h>

#define LOGTAG "RTC"

int64_t RTC_on_setup(alarm_id_t id, void *data);
void RTC_setup();
void RTC_read(void *data);
void RTC_write(void *data);
bool RTC_on_update(repeating_timer_t *rt);
uint8_t dow(uint16_t year, uint8_t month, uint8_t day);
uint8_t weekday2dow(uint8_t weekday);

struct {
    bool initialised;
    bool ready;

    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dow;

    repeating_timer_t timer;
    mutex_t guard;
} RTC = {
    .initialised = false,
    .ready = false,

    .year = 2020,
    .month = 1,
    .day = 1,
    .hour = 0,
    .minute = 0,
    .second = 0,
    .dow = 0x08, // Wednesday
};

/*
 * Initialises the RX8900SA.
 */
void RTC_init() {
    infof(LOGTAG, "init");

    // ... initialise RX8900SA
    int err;

    if ((err = RX8900SA_init(U5)) == ERR_OK) {
        RTC.initialised = true;
        RTC.ready = false;
    } else if (err == ERR_RX8900SA_VLF) {
        RTC.initialised = false;
        RTC.ready = false;

        add_alarm_in_ms(RX8900SA_tSTA, RTC_on_setup, NULL, true);
    }

    mutex_init(&RTC.guard);

    // ... initialise on-board RTC
    datetime_t t = {
        .year = 2020,
        .month = 1,
        .day = 1,
        .dotw = 3,
        .hour = 0,
        .min = 0,
        .sec = 0,
    };

    infof(LOGTAG, "initialised %p", &RTC);
}

/*
 * Initialises RX8900SA after tSTA.
 */
int64_t RTC_on_setup(alarm_id_t id, void *data) {
    closure task = {
        .f = RTC_setup,
        .data = &RTC,
    };

    I2C0_push(&task);

    return 0;
}

/*
 * Initialises RX8900SA.
 */
void RTC_setup() {
    int err;

    if ((err = RX8900SA_setup(U5)) == ERR_OK) {
        RTC.initialised = true;
        RTC.ready = false;
        infof(LOGTAG, "ready");
    } else {
        warnf(LOGTAG, "setup failed");
    }
}

/*
 * Synchronizes on-board RTC to RX8900SA every 60s.
 */
void RTC_start() {
    infof(LOGTAG, "start");

    closure task = {
        .f = RTC_read,
        .data = &RTC,
    };

    I2C0_push(&task);
    add_repeating_timer_ms(400, RTC_on_update, NULL, &RTC.timer);
}

bool RTC_on_update(repeating_timer_t *rt) {
    closure task = {
        .f = RTC_read,
        .data = &RTC,
    };

    I2C0_push(&task);

    return true;
}

/*
 * I2C0 task to update RTC struct from RX8900SA.
 */
void RTC_read(void *data) {
    if (RTC.initialised) {
        uint16_t year = 0;
        uint8_t month = 0;
        uint8_t day = 0;
        uint8_t hour = 0;
        uint8_t minute = 0;
        uint8_t second = 0;
        uint8_t weekday = 0;
        int err;

        if ((err = RX8900SA_get_datetime(U5, &year, &month, &day, &hour, &minute, &second, &weekday)) != ERR_OK) {
            syserr_set(ERR_RX8900SA, LOGTAG, "get-datetime error %d", err);
        } else {
            mutex_enter_blocking(&RTC.guard);
            RTC.year = year;
            RTC.month = month;
            RTC.day = day;
            RTC.hour = hour;
            RTC.minute = minute;
            RTC.second = second;
            RTC.dow = weekday;

            if (!RTC.ready) {
                RTC.ready = true;
            }
            mutex_exit(&RTC.guard);
        }
    }
}

/*
 * I2C0 task to set RX8900SA.
 */
void RTC_write(void *data) {
    datetime *dt = (datetime *)data;

    if (RTC.initialised) {
        if (dt->tag == RTC_SET_DATE) {
            uint16_t year = dt->yyyymmdd.year;
            uint8_t month = dt->yyyymmdd.month;
            uint8_t day = dt->yyyymmdd.day;
            uint8_t dow = dt->yyyymmdd.dow;
            int err;

            if ((err = RX8900SA_set_date(U5, year, month, day)) != ERR_OK) {
                warnf(LOGTAG, "set-date error %d", err);
            } else if ((err = RX8900SA_set_dow(U5, dow)) != ERR_OK) {
                warnf(LOGTAG, "set-date error %d", err);
            }
        }

        if (dt->tag == RTC_SET_TIME) {
            uint8_t hour = dt->HHmmss.hour;
            uint8_t minute = dt->HHmmss.minute;
            uint8_t second = dt->HHmmss.second;
            int err;

            if ((err = RX8900SA_set_time(U5, hour, minute, second)) != ERR_OK) {
                warnf(LOGTAG, "set-time error %d", err);
            }
        }

        if (dt->tag == RTC_SET_DATETIME) {
            uint16_t year = dt->datetime.year;
            uint8_t month = dt->datetime.month;
            uint8_t day = dt->datetime.day;
            uint8_t hour = dt->datetime.hour;
            uint8_t minute = dt->datetime.minute;
            uint8_t second = dt->datetime.second;
            uint8_t dow = dt->datetime.dow;
            int err;

            if ((err = RX8900SA_set_datetime(U5, year, month, day, hour, minute, second, dow)) != ERR_OK) {
                warnf(LOGTAG, "set-datetime error %d", err);
            }
        }
    }

    datetime_free(dt);
}

/*
 * Resets the RTC.
 */
void RTC_reset() {
    infof(LOGTAG, "reset");

    RTC.initialised = false;
    RTC.ready = false;

    RX8900SA_reset(U5);
    add_alarm_in_ms(RX8900SA_tSTA, RTC_on_setup, NULL, true);
}

/*
 * Returns true if the RX8900SA has been initialised.
 */
bool RTC_ready() {
    return RTC.initialised && RTC.ready;
}

bool RTC_set_date(uint16_t year, uint8_t month, uint8_t day) {
    debugf(LOGTAG, "set-date %04u-%02u-%02u  %s", year, month, day, RTC.initialised ? "" : "-- not initialised --");

    if (RTC.initialised) {
        uint8_t weekday = dow(year, month, day);
        datetime *dt = datetime_alloc();

        if (dt != NULL) {
            dt->tag = RTC_SET_DATE;
            dt->yyyymmdd.year = year;
            dt->yyyymmdd.month = month;
            dt->yyyymmdd.day = day;
            dt->yyyymmdd.dow = weekday;

            closure write = {
                .f = RTC_write,
                .data = dt,
            };

            closure read = {
                .f = RTC_read,
                .data = &RTC,
            };

            if (!I2C0_push(&write)) {
                datetime_free(dt);
            } else if (I2C0_push(&read)) {
                return true;
            }
        }
    }

    return false;
}

bool RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    debugf(LOGTAG, "set-time %02u-%02u-%02u  %s", hour, minute, second, RTC.initialised ? "" : "-- not initialised --");

    if (RTC.initialised) {
        datetime *dt = datetime_alloc();

        if (dt != NULL) {
            dt->tag = RTC_SET_TIME;
            dt->HHmmss.hour = hour;
            dt->HHmmss.minute = minute;
            dt->HHmmss.second = second;

            closure write = {
                .f = RTC_write,
                .data = dt,
            };

            closure read = {
                .f = RTC_read,
                .data = &RTC,
            };

            if (!I2C0_push(&write)) {
                datetime_free(dt);
            } else if (I2C0_push(&read)) {
                return true;
            }
        }
    }

    return false;
}

bool RTC_get_datetime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *dow) {
    if (RTC.initialised && RTC.ready) {
        mutex_enter_blocking(&RTC.guard);

        if (year != NULL) {
            *year = RTC.year;
        }

        if (month != NULL) {
            *month = RTC.month;
        }

        if (day != NULL) {
            *day = RTC.day;
        }

        if (hour != NULL) {
            *hour = RTC.hour;
        }

        if (minute != NULL) {
            *minute = RTC.minute;
        }

        if (second != NULL) {
            *second = RTC.second;
        }

        if (dow != NULL) {
            *dow = RTC.dow;
        }

        mutex_exit(&RTC.guard);

        return true;
    }

    return false;
}

bool RTC_set_datetime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
    debugf(LOGTAG, "set-datetime %04u-%02u-%02u %02u:%02u:%02u %s",
           year, month, day,
           hour, minute, second,
           RTC.initialised ? "" : "-- not initialised --");

    if (RTC.initialised) {
        uint8_t weekday = dow(year, month, day);
        datetime *dt = datetime_alloc();

        if (dt != NULL) {
            dt->tag = RTC_SET_DATETIME;
            dt->datetime.year = year;
            dt->datetime.month = month;
            dt->datetime.day = day;
            dt->datetime.dow = weekday;
            dt->datetime.hour = hour;
            dt->datetime.minute = minute;
            dt->datetime.second = second;

            closure write = {
                .f = RTC_write,
                .data = dt,
            };

            closure read = {
                .f = RTC_read,
                .data = &RTC,
            };

            if (!I2C0_push(&write)) {
                datetime_free(dt);
            } else if (I2C0_push(&read)) {
                return true;
            }
        }
    }

    return false;
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

uint8_t weekday2dow(uint8_t weekday) {
    if (weekday == SUNDAY) {
        return 0;
    } else if (weekday == MONDAY) {
        return 1;
    } else if (weekday == TUESDAY) {
        return 2;
    } else if (weekday == WEDNESDAY) {
        return 3;
    } else if (weekday == THURSDAY) {
        return 4;
    } else if (weekday == FRIDAY) {
        return 5;
    } else if (weekday == SATURDAY) {
        return 6;
    } else {
        return 0;
    }
}
