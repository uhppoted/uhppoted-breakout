#include <stdio.h>
#include <string.h>

#include <I2C0.h>
#include <I2C1.h>
#include <RTC.h>

void get_date();
void set_date();
void get_time();
void set_time();
void reset();
void help();
void debug();

void exec(char *cmd) {
    char s[128];

    printf("\n");

    if (strncasecmp(cmd, "get date", 8) == 0) {
        get_date();
    } else if (strncasecmp(cmd, "set date", 8) == 0) {
        set_date();
    } else if (strncasecmp(cmd, "get time", 8) == 0) {
        get_time();
    } else if (strncasecmp(cmd, "set time", 8) == 0) {
        set_time();
    } else if (strncasecmp(cmd, "reset", 8) == 0) {
        reset();
    } else if (strncasecmp(cmd, "help", 4) == 0) {
        help();
    } else if (strncasecmp(cmd, "x", 1) == 0) {
        debug();
    }
}

void debug() {
    I2C0_scan();
    I2C1_scan();
}

void reset() {
    RTC_reset();
}

void get_date() {
    char date[11] = {0};

    RTC_get_date(date);

    printf(">>> DATE:%s\n", date);
}

void set_date() {
    uint16_t year = 2024;
    uint8_t month = 5;
    uint8_t day = 24;

    RTC_set_date(year, month, day);
}

void get_time() {
    char time[11] = {0};

    RTC_get_time(time);

    printf(">>> TIME:%s\n", time);
}

void set_time() {
    uint8_t hour = 12;
    uint8_t minute = 34;
    uint8_t second = 56;

    RTC_set_time(hour, minute, second);
}

void help() {
    printf("BREAKOUT Rev.0\n");
    printf("\n");
    printf("Commands:\n");
    printf("  get date\n");
    printf("  set date\n");
    printf("  get time\n");
    printf("  set time\n");
    printf("  reset\n");
    printf("\n");
}