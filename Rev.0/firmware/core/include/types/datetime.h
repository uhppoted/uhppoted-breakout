#pragma once

typedef enum {
    RTC_UNKNOWN,
    RTC_SET_DATE,
    RTC_SET_TIME,
    RTC_SET_DATETIME,
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

        struct {
            uint16_t year;
            uint8_t month;
            uint8_t day;
            uint8_t dow;
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        } datetime;
    };
} datetime;
