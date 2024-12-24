#include <stdio.h>
#include <stdlib.h>

#include <hardware/rtc.h>
#include <pico/stdlib.h>
#include <pico/sync.h>

#include <I2C0.h>
#include <RTC.h>
#include <RX8900SA.h>
#include <breakout.h>
#include <log.h>
#include <state.h>
#include <trace.h>

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

typedef enum {
    RTC_UNKNOWN,
    RTC_SET_DATE,
    RTC_SET_TIME,
} RTC_TASK;

typedef struct datetime {
    RTC_TASK tag;
    union {
        struct {
            uint16_t year;
            uint8_t month;
            uint8_t day;
            uint8_t dow;
        } yyyymmdd;

        struct {
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        } HHmmss;
    };
} datetime;

/*
 * Initialises the RX8900SA.
 */
void RTC_init() {
    infof("RTC", "init");

    // ... initialise RX8900SA
    int err;

    if ((err = RX8900SA_init(U5)) == ERR_OK) {
        RTC.initialised = true;
        RTC.ready = false;
    } else if (err == ERR_VLF) {
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

    rtc_init();
    sleep_us(64);
    rtc_set_datetime(&t);

    infof("RTC", "initialised %p", &RTC);
}

/*
 * Initialises RX8900SA after tSTA.
 */
int64_t RTC_on_setup(alarm_id_t id, void *data) {
    closure task = {
        .f = RTC_setup,
        .data = &RTC,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "RTC", "setup: queue full");
    }

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
        infof("RTC", "ready");
    } else {
        warnf("RTC", "setup failed");
    }
}

/*
 * Synchronizes on-board RTC to RX8900SA every 60s.
 */
void RTC_start() {
    infof("RTC", "start");

    closure task = {
        .f = RTC_read,
        .data = &RTC,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "RTC", "update: queue full");
    }

    add_repeating_timer_ms(60000, RTC_on_update, NULL, &RTC.timer);
}

bool RTC_on_update(repeating_timer_t *rt) {
    uint32_t trace = trace_in(TRACE_RTC_TICK);

    closure task = {
        .f = RTC_read,
        .data = &RTC,
    };

    if (!I2C0_push(&task)) {
        set_error(ERR_QUEUE_FULL, "RTC", "update: queue full");
    }

    trace_out(TRACE_RTC_TICK, trace);

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
        bool ok = true;
        int err;

        if (mutex_try_enter(&RTC.guard, NULL)) {

            if ((err = RX8900SA_get_date(U5, &year, &month, &day)) != ERR_OK) {
                set_error(ERR_RX8900SA, "RTC", "get-date error %d", err);
                ok = false;
            } else {
                RTC.year = year;
                RTC.month = month;
                RTC.day = day;
            }

            if ((err = RX8900SA_get_time(U5, &hour, &minute, &second)) != ERR_OK) {
                set_error(ERR_RX8900SA, "RTC", "get-time error %d", err);
                ok = false;
            } else {
                RTC.hour = hour;
                RTC.minute = minute;
                RTC.second = second;
            }

            if ((err = RX8900SA_get_dow(U5, &weekday)) != ERR_OK) {
                set_error(ERR_RX8900SA, "RTC", "get-dow error %d", err);
                ok = false;
            } else {
                RTC.dow = weekday;
            }

            if (!RTC.ready && ok) {
                RTC.ready = true;
                infof("RTC", "READY %04d-%02d-%02d %02d:%02d:%02d", RTC.year, RTC.month, RTC.day, RTC.hour, RTC.minute, RTC.second);
            }

            // // ... update onboard RTC
            if (ok) {
                datetime_t t = {
                    .year = RTC.year,
                    .month = RTC.month,
                    .day = RTC.day,
                    .dotw = weekday2dow(weekday),
                    .hour = RTC.hour,
                    .min = RTC.minute,
                    .sec = RTC.second,
                };

                rtc_set_datetime(&t);
            }

            mutex_exit(&RTC.guard);
        }
    }
}

/*
 * I2C0 task to set RX8900SA.
 */
void RTC_write(void *data) {
    datetime *d = (datetime *)data;

    if (RTC.initialised) {
        if (d->tag == RTC_SET_DATE) {
            uint16_t year = d->yyyymmdd.year;
            uint8_t month = d->yyyymmdd.month;
            uint8_t day = d->yyyymmdd.day;
            uint8_t dow = d->yyyymmdd.dow;
            int err;

            if ((err = RX8900SA_set_date(U5, year, month, day)) != ERR_OK) {
                warnf("RTC", "set-date error %d", err);
            } else if ((err = RX8900SA_set_dow(U5, dow)) != ERR_OK) {
                warnf("RTC", "set-date error %d", err);
            } else if (mutex_try_enter(&RTC.guard, NULL)) {
                RTC.year = year;
                RTC.month = month;
                RTC.day = day;
                RTC.dow = dow;
                mutex_exit(&RTC.guard);
            }
        }

        if (d->tag == RTC_SET_TIME) {
            uint8_t hour = d->HHmmss.hour;
            uint8_t minute = d->HHmmss.minute;
            uint8_t second = d->HHmmss.second;
            int err;

            if ((err = RX8900SA_set_time(U5, hour, minute, second)) != ERR_OK) {
                warnf("RTC", "set-time error %d", err);
            } else if (mutex_try_enter(&RTC.guard, NULL)) {
                RTC.hour = hour;
                RTC.minute = minute;
                RTC.second = second;
                mutex_exit(&RTC.guard);
            }
        }
    }

    free(data);
}

/*
 * Resets the RTC.
 */
void RTC_reset() {
    infof("RTC", "reset");

    RTC.initialised = false;
    RTC.ready = false;

    RX8900SA_reset(U5);
    add_alarm_in_ms(RX8900SA_tSTA, RTC_on_setup, NULL, true);
}

void RTC_get_date(char *yymmmdd, int N) {
    if (RTC.initialised && RTC.ready) {
        // mutex_enter_blocking(&RTC.guard);
        // uint16_t year = RTC.year;
        // uint8_t month = RTC.month;
        // uint8_t day = RTC.day;
        // mutex_exit(&RTC.guard);

        datetime_t dt;

        rtc_get_datetime(&dt);

        uint16_t year = dt.year;
        uint8_t month = dt.month;
        uint8_t day = dt.day;

        snprintf(yymmmdd, N, "%04u-%02u-%02u", year, month, day);
    } else {
        snprintf(yymmmdd, N, "---- -- --");
    }
}

void RTC_set_date(uint16_t year, uint8_t month, uint8_t day) {
    if (RTC.initialised) {
        uint8_t weekday = dow(year, month, day);
        datetime *dt = (datetime *)calloc(1, sizeof(datetime));

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
            set_error(ERR_QUEUE_FULL, "RTC", "set-date: queue full");
        }

        if (!I2C0_push(&read)) {
            set_error(ERR_QUEUE_FULL, "RTC", "update: queue full");
        }
    }
}

void RTC_get_time(char *HHmmss, int N) {
    if (RTC.initialised && RTC.ready) {
        // mutex_enter_blocking(&RTC.guard);
        // uint8_t hour = RTC.hour;
        // uint8_t minute = RTC.minute;
        // uint8_t second = RTC.second;
        // mutex_exit(&RTC.guard);

        datetime_t dt;

        rtc_get_datetime(&dt);

        uint8_t hour = dt.hour;
        uint8_t minute = dt.min;
        uint8_t second = dt.sec;

        snprintf(HHmmss, N, "%02u:%02u:%02u", hour, minute, second);
    } else {
        snprintf(HHmmss, N, "--:--:--");
    }
}

void RTC_set_time(uint8_t hour, uint8_t minute, uint8_t second) {
    if (RTC.initialised) {
        datetime *dt = (datetime *)calloc(1, sizeof(datetime));

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
            set_error(ERR_QUEUE_FULL, "RTC", "set-time: queue full");
        }

        if (!I2C0_push(&read)) {
            set_error(ERR_QUEUE_FULL, "RTC", "set-time: queue full");
        }
    }
}

void RTC_get_dow(char *weekday, int N) {
    if (RTC.initialised && RTC.ready) {
        // mutex_enter_blocking(&RTC.guard);
        // uint8_t dow = RTC.dow;
        // mutex_exit(&RTC.guard);
        //
        // if (RTC.dow == SUNDAY) {
        //     snprintf(weekday, N, "Sunday");
        // } else if (RTC.dow == MONDAY) {
        //     snprintf(weekday, N, "Monday");
        // } else if (RTC.dow == TUESDAY) {
        //     snprintf(weekday, N, "Tuesday");
        // } else if (RTC.dow == WEDNESDAY) {
        //     snprintf(weekday, N, "Wednesday");
        // } else if (RTC.dow == THURSDAY) {
        //     snprintf(weekday, N, "Thursday");
        // } else if (RTC.dow == FRIDAY) {
        //     snprintf(weekday, N, "Friday");
        // } else if (RTC.dow == SATURDAY) {
        //     snprintf(weekday, N, "Saturday");
        // } else {
        //     snprintf(weekday, N, "???");
        // }

        datetime_t dt;

        rtc_get_datetime(&dt);

        if (dt.dotw == 0) {
            snprintf(weekday, N, "Sunday");
        } else if (dt.dotw == 1) {
            snprintf(weekday, N, "Monday");
        } else if (dt.dotw == 2) {
            snprintf(weekday, N, "Tuesday");
        } else if (dt.dotw == 3) {
            snprintf(weekday, N, "Wednesday");
        } else if (dt.dotw == 4) {
            snprintf(weekday, N, "Thursday");
        } else if (dt.dotw == 5) {
            snprintf(weekday, N, "Friday");
        } else if (dt.dotw == 6) {
            snprintf(weekday, N, "Saturday");
        } else {
            snprintf(weekday, N, "???");
        }
    } else {
        snprintf(weekday, N, "---");
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