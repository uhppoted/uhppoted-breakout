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

    slice octets = {
        .capacity = 32,
        .length = 0,
        .bytes = (char *)calloc(32, sizeof(uint8_t)),
    };

    // ... delay to let RTC time (hopefully) propagate
    sleep_ms(5);

    // ... return actual RTC time
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

    printf(">>> %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
    printf(">>> %04d-%02d-%02d %02d:%02d:%02d\n", yyyy, mm, dd, HH, MM, SS);

    N = snprintf(octets.bytes, octets.capacity, "%04u-%02u-%02u %02u:%02u:%02u", yyyy, mm, dd, HH, MM, SS);
    octets.length = N < 0 ? 0 : (N < 19 ? N : 19);

    v->tag = VALUE_OCTET_STRING;
    v->octets = octets;

    return SSMP_ERROR_NONE;
}

// bool is_leap_year(int year) {
//     return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
// }
//
// uint64_t datetime_to_epoch(const datetime_t* dt) {
//     const int days_in_month[] = {
//         0, 31, 28, 31, 30, 31, 30,
//         31, 31, 30, 31, 30, 31
//     };
//
//     uint64_t seconds = 0;
//
//     for (int y = 1970; y < dt->year; y++) {
//         seconds += (is_leap_year(y) ? 366 : 365) * 86400;
//     }
//
//     for (int m = 1; m < dt->month; m++) {
//         if (m == 2 && is_leap_year(dt->year))
//             seconds += 29 * 86400;
//         else
//             seconds += days_in_month[m] * 86400;
//     }
//
//     seconds += (dt->day - 1) * 86400;
//     seconds += dt->hour * 3600;
//     seconds += dt->min * 60;
//     seconds += dt->sec;
//
//     return seconds;
// }
