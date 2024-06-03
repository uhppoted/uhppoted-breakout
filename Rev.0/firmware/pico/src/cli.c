#include <stdio.h>
#include <string.h>

#include <I2C0.h>
#include <I2C1.h>
#include <RTC.h>
#include <U2.h>
#include <U3.h>
#include <U4.h>

void get_date();
void set_date();
void get_time();
void set_time();
void get_weekday();
void set_relay(const char *relay, bool state);
void set_LED(const char *led, bool state);
void reset();
void scan();
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
    } else if (strncasecmp(cmd, "get weekday", 11) == 0) {
        get_weekday();
    } else if (strncasecmp(cmd, "set relay ", 10) == 0) {
        set_relay(&cmd[10], true);
    } else if (strncasecmp(cmd, "clear relay ", 12) == 0) {
        set_relay(&cmd[11], false);
    } else if (strncasecmp(cmd, "set LED ", 8) == 0) {
        set_LED(&cmd[8], true);
    } else if (strncasecmp(cmd, "clear LED ", 10) == 0) {
        set_LED(&cmd[10], false);
    } else if (strncasecmp(cmd, "reset", 5) == 0) {
        reset();
    } else if (strncasecmp(cmd, "scan", 4) == 0) {
        scan();
    } else if (strncasecmp(cmd, "help", 4) == 0) {
        help();
    } else if (strncasecmp(cmd, "x", 1) == 0) {
        debug();
    } else {
        printf(">>>> WTF???? %s\n", cmd);
    }
}

void debug() {
    U4_debug();
}

void reset() {
    RTC_reset();
}

void get_date() {
    char date[11] = {0};

    RTC_get_date(date, 11);

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

    RTC_get_time(time, 11);

    printf(">>> TIME:%s\n", time);
}

void get_weekday() {
    char weekday[9] = {0};

    RTC_get_dow(weekday, 9);

    printf(">>> DOW:%s\n", weekday);
}

void set_time() {
    uint8_t hour = 12;
    uint8_t minute = 34;
    uint8_t second = 56;

    RTC_set_time(hour, minute, second);
}

void set_relay(const char *cmd, bool state) {
    int N = strlen(cmd);

    if (N > 0) {
        uint8_t relay;
        int rc;

        if ((rc = sscanf(cmd, "%u", &relay)) < 1) {
            return;
        } else {
            U4_set_relay(relay, state);
        }
    }
}

void set_LED(const char *cmd, bool state) {
    int N = strlen(cmd);

    if (N > 0) {
        uint8_t LED;
        int rc;

        if ((rc = sscanf(cmd, "%u", &LED)) >= 1) {
            U4_set_LED(LED, state);
            printf("ok\n");
            return;
        }

        if (strncasecmp(cmd, "ERR", 3) == 0) {
            U4_set_ERR(state);
            printf("ok\n");
            return;
        }

        if (strncasecmp(cmd, "IN", 2) == 0) {
            U4_set_IN(state);
            printf("ok\n");
            return;
        }

        if (strncasecmp(cmd, "SYS", 3) == 0) {
            U4_set_SYS(state);
            printf("ok\n");
            return;
        }
    }
}

void scan() {
    I2C0_scan();
    I2C1_scan();
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