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
    char datetime[20] = {0};

    RTC_get_datetime(datetime, sizeof(datetime));

    int N = snprintf(octets.bytes, octets.capacity, "%s", datetime);
    if (N > 0) {
        octets.length = N;
    }

    v->tag = VALUE_OCTET_STRING;
    v->octets = octets;

    return SSMP_ERROR_NONE;
}
