#include <inttypes.h>
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

const int32_t RTC_SYNC_INTERVAL = 15000;
const int32_t RTC_TICK_INTERVAL = 500;
const double Kp = 0.00015 * (double)RTC_TICK_INTERVAL / 1000.0;
const double Ki = 0.00005 * (double)RTC_TICK_INTERVAL / 1000.0;
const double Kd = 0.0000125 * (double)RTC_TICK_INTERVAL / 1000.0;

int64_t RTC_on_setup(alarm_id_t id, void *data);
void RTC_setup();
void RTC_read(void *data);
void RTC_write(void *data);
bool RTC_on_update(repeating_timer_t *rt);
bool RTC_on_tick(repeating_timer_t *rt);
uint8_t dow(uint16_t year, uint8_t month, uint8_t day);
uint8_t weekday2dow(uint8_t weekday);

void epoch_to_datetime(uint64_t, uint16_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);

struct {
    bool initialised;
    bool ready;

    repeating_timer_t sync;
    repeating_timer_t tick;
    uint64_t last_tick;
    uint64_t epoch;
    double k;
    double integral;
    double error;

    struct {
        uint32_t counter;
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        double delta;
    } trace;

    mutex_t guard;
} RTC = {
    .initialised = false,
    .ready = false,

    .last_tick = 0,
    .epoch = 0,
    .k = 1.0,
    .integral = 0.0,
    .error = 0.0,

    .trace = {
        .counter = 0,
        .year = 0,
        .month = 0,
        .day = 0,
        .hour = 0,
        .minute = 0,
        .day = 0,
        .delta = 0.0,
    },
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

    add_repeating_timer_ms(RTC_SYNC_INTERVAL, RTC_on_update, NULL, &RTC.sync);
    add_repeating_timer_ms(RTC_TICK_INTERVAL, RTC_on_tick, NULL, &RTC.tick);
}

/*
 * Queues a task to get the current time from the RX8900SA.
 */
bool RTC_on_update(repeating_timer_t *rt) {
    closure task = {
        .f = RTC_read,
        .data = &RTC,
    };

    I2C0_push(&task);

    return true;
}

/*
 * Updates the in-memory date/time from the microsecond hardware counter.
 */
bool RTC_on_tick(repeating_timer_t *rt) {
    if (RTC.initialised && RTC.ready && RTC.epoch != 0) {
        uint64_t now = time_us_64();

        if (RTC.last_tick != 0) {
            int64_t delta = now - RTC.last_tick;
            double dt = RTC.k * (double)delta;

            RTC.epoch += (int64_t)dt;
        }

        RTC.last_tick = now;
    }

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
            if (!RTC.ready) {
                RTC.ready = true;
            }

            uint64_t epoch = datetime_to_epoch(year, month, day, hour, minute, second);
            uint64_t µs = 500000; // RTC.timers.epoch % 1000000;

            if (RTC.epoch == 0) {
                RTC.epoch = 1000000 * epoch + µs;
            } else {
                int64_t error = (int64_t)(1000000 * epoch) - (int64_t)RTC.epoch;
                double e = (double)error / 1000.0;
                double sum = RTC.integral + e;
                double gradient = e - RTC.error;
                double delta = Kp * e + Ki * sum + Kd * gradient;
                double k = 1.0 + delta;

                RTC.integral = sum;
                RTC.error = e;

                RTC.trace.counter++;
                RTC.trace.year = year;
                RTC.trace.month = month;
                RTC.trace.day = day;
                RTC.trace.hour = hour;
                RTC.trace.minute = minute;
                RTC.trace.second = second;
                RTC.trace.delta = delta;

                if (k < 0.75) {
                    RTC.k = 0.75;
                } else if (k > 1.25) {
                    RTC.k = 1.25;
                } else {
                    RTC.k = k;
                }
            }

            mutex_exit(&RTC.guard);
        }
    }
}

/*
 * Prints out the current state of the RTC.
 */
void RTC_trace() {
    static uint32_t counter = 0;

    if (counter != RTC.trace.counter) {
        counter = RTC.trace.counter;

        debugf(LOGTAG, "trace  %04u-%02u-%02u %02u:%02u:%02u error:%-+8.3llf delta:%-+8.3llf k:%.3llf",
               RTC.trace.year,
               RTC.trace.month,
               RTC.trace.day,
               RTC.trace.hour,
               RTC.trace.minute,
               RTC.trace.second,
               RTC.error,
               RTC.trace.delta,
               RTC.k);
    }
}

/*
 * I2C0 task to set RX8900SA.
 */
void RTC_write(void *data) {
    datetime *dt = (datetime *)data;

    if (RTC.initialised) {
        uint16_t year = dt->year;
        uint8_t month = dt->month;
        uint8_t day = dt->day;
        uint8_t hour = dt->hour;
        uint8_t minute = dt->minute;
        uint8_t second = dt->second;
        uint8_t dow = dt->dow;
        int err;

        if ((err = RX8900SA_set_datetime(U5, year, month, day, hour, minute, second, dow)) != ERR_OK) {
            warnf(LOGTAG, "set-datetime error %d", err);
        } else {
            // uint64_t epoch = datetime_to_epoch(year, month, day, hour, minute, second);
            // uint64_t µs = 500000; // RTC.timers.epoch % 1000000;
            //
            // RTC.epoch = (1000000 * epoch) + µs;

            mutex_enter_blocking(&RTC.guard);
            RTC.ready = false;
            RTC.epoch = 0;
            RTC.last_tick = 0;
            mutex_exit(&RTC.guard);
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

    if (RTC.initialised && RTC.ready) {
        uint8_t hour;
        uint8_t minute;
        uint8_t second;

        if (!RTC_get_datetime(NULL, NULL, NULL, &hour, &minute, &second, NULL)) {
            warnf(LOGTAG, "set-date error (epoch conversion)");
            return false;
        }

        datetime *dt = datetime_alloc();
        if (dt != NULL) {
            dt->year = year;
            dt->month = month;
            dt->day = day;
            dt->dow = dow(year, month, day);
            dt->hour = hour;
            dt->minute = minute;
            dt->second = second;

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
    debugf(LOGTAG, "set-time %02u:%02u:%02u  %s", hour, minute, second, RTC.initialised ? "" : "-- not initialised --");

    if (RTC.initialised && RTC.ready) {
        uint16_t year;
        uint8_t month;
        uint8_t day;

        if (!RTC_get_datetime(&year, &month, &day, NULL, NULL, NULL, NULL)) {
            warnf(LOGTAG, "set-time error (epoch conversion)");
            return false;
        }

        datetime *dt = datetime_alloc();
        if (dt != NULL) {
            dt->year = year;
            dt->month = month;
            dt->day = day;
            dt->dow = dow(year, month, day);
            dt->hour = hour;
            dt->minute = minute;
            dt->second = second;

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

bool RTC_get_datetime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *weekday) {
    if (RTC.initialised && RTC.ready) {
        mutex_enter_blocking(&RTC.guard);

        uint16_t yyyy;
        uint8_t mmm;
        uint8_t dd;
        uint8_t HH;
        uint8_t mm;
        uint8_t ss;

        uint64_t epoch = RTC.epoch / 1000000; // convert µs to s

        epoch_to_datetime(epoch, &yyyy, &mmm, &dd, &HH, &mm, &ss);

        if (year != NULL) {
            *year = yyyy;
        }

        if (month != NULL) {
            *month = mmm;
        }

        if (day != NULL) {
            *day = dd;
        }

        if (hour != NULL) {
            *hour = HH;
        }

        if (minute != NULL) {
            *minute = mm;
        }

        if (second != NULL) {
            *second = ss;
        }

        if (weekday != NULL) {
            *weekday = dow(yyyy, mmm, dd);
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
        datetime *dt = datetime_alloc();

        if (dt != NULL) {
            dt->year = year;
            dt->month = month;
            dt->day = day;
            dt->dow = dow(year, month, day);
            dt->hour = hour;
            dt->minute = minute;
            dt->second = second;

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

uint64_t datetime_to_epoch(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    bool is_leap(int y) {
        return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
    }

    const int days_in_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31};

    uint64_t days = 0;

    // ... years since 1970
    for (int y = 1970; y < year; y++) {
        days += is_leap(y) ? 366 : 365;
    }

    // ... months this year (1–(mon-1))
    for (int m = 1; m < mon; m++) {
        days += days_in_month[m - 1];
        if (m == 2 && is_leap(year))
            days += 1;
    }

    days += day - 1;

    return (uint64_t)(days * 86400 + hour * 3600 + min * 60 + sec);
}

void epoch_to_datetime(uint64_t epoch, uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second) {
    bool is_leap(int y) {
        return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
    }

    const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    uint64_t days = epoch / 86400;
    uint64_t rem = epoch % 86400;

    if (rem < 0) {
        rem += 86400;
        days -= 1;
    }

    *hour = rem / 3600;
    rem %= 3600;
    *minute = rem / 60;
    *second = rem % 60;

    int y = 1970;
    int m;

    while (1) {
        int yd = is_leap(y) ? 366 : 365;
        if (days >= yd) {
            days -= yd;
            y++;
        } else {
            break;
        }
    }

    for (m = 0; m < 12; m++) {
        int dim = days_in_month[m];
        if (m == 1 && is_leap(y))
            dim += 1;
        if (days >= dim) {
            days -= dim;
        } else {
            break;
        }
    }

    *year = y;
    *month = m + 1;
    *day = days + 1;
}
