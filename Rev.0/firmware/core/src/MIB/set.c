#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        if ((hash == item.hash) && (strcmp(OID, item.OID) == 0) && (item.set != NULL)) {
            return item.set(u, v);
        }
    }

    return SSMP_ERROR_NO_SUCH_OBJECT;
}

int64_t MIB_set_datetime(const value u, value *v) {
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

    // NTS: RTC is not updated immediately
    // char date[16] = {0};
    // char time[16] = {0};
    // char datetime[32] = {0};
    //
    // RTC_get_date(date, sizeof(date));
    // RTC_get_time(time, sizeof(time));
    //
    // snprintf(datetime, sizeof(datetime), "%s %s", date, time);

    int N = snprintf(octets.bytes, octets.capacity, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
    if (N > 0) {
        octets.length = N;
    }

    v->tag = VALUE_OCTET_STRING;
    v->octets = octets;

    return SSMP_ERROR_NONE;
}
