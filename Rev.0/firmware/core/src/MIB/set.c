#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/time.h>

#include <crypt/hash/djb2.h>

#include <MIB.h>
#include <RTC.h>
#include <SSMP.h>
#include <log.h>

#define LOGTAG "MIB"

int64_t datetime_to_epoch(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

int64_t MIB_set(const char *OID, const value u, value *v) {
    uint32_t hash = djb2(OID);

    int N = sizeof(OIDs) / sizeof(MIBItem);
    for (int i = 0; i < N; i++) {
        MIBItem item = OIDs[i];

        if (hash == item.hash && strcmp(OID, item.OID) == 0) {
            if (item.set == NULL) {
                return SSMP_ERROR_READONLY;
            }

            return item.set(u, v);
        }
    }

    return SSMP_ERROR_NO_SUCH_OBJECT;
}

int64_t MIB_set_datetime(const value u, value *v) {
    // ... check value type
    if (u.tag != VALUE_OCTET_STRING) {
        return SSMP_ERROR_WRONG_TYPE;
    }

    // ... RTC initialised?
    if (!RTC_ready()) {
        return SSMP_ERROR_NOT_WRITABLE;
    }

    // ... set RTC time
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int rc;

    const char *s = u.octets.bytes;

    if ((rc = sscanf(s, "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second)) != 6) {
        return SSMP_ERROR_BAD_VALUE;
    } else if (!RTC_set_datetime(year, month, day, hour, minute, second)) {
        warnf(LOGTAG, "error setting RTC datetime %04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
        return SSMP_ERROR_COMMIT_FAILED;
    }

    // ... delay to let RTC time (hopefully) propagate
    sleep_ms(5);

    // ... get current RTC time
    uint16_t yyyy;
    uint8_t mm;
    uint8_t dd;
    uint8_t HH;
    uint8_t MM;
    uint8_t SS;
    int N;

    if (!RTC_get_datetime(&yyyy, &mm, &dd, &HH, &MM, &SS)) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    // ... check delta < 1s
    int64_t t1 = datetime_to_epoch(year, month, day, hour, minute, second);
    int64_t t2 = datetime_to_epoch(yyyy, mm, dd, HH, MM, SS);
    int64_t dt = t1 < t2 ? t2 - t1 : t1 - t2;

    if (dt > 1) {
        return SSMP_ERROR_COMMIT_FAILED;
    }

    // ... return current RTC time
    slice octets = {
        .capacity = 32,
        .length = 0,
        .bytes = (char *)calloc(32, sizeof(uint8_t)),
    };

    N = snprintf(octets.bytes, octets.capacity, "%04u-%02u-%02u %02u:%02u:%02u", yyyy, mm, dd, HH, MM, SS);
    octets.length = N < 0 ? 0 : (N < 19 ? N : 19);

    v->tag = VALUE_OCTET_STRING;
    v->octets = octets;

    return SSMP_ERROR_NONE;
}

int64_t datetime_to_epoch(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    bool is_leap(int y) {
        return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
    }

    const int days_in_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31};

    int64_t days = 0;

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

    return (int64_t)(days * 86400 + hour * 3600 + min * 60 + sec);
}
