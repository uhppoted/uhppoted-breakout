#include <stdio.h>
#include <string.h>

#include <PCAL6408APW.h>
#include <RX8900SA.h>
#include <breakout.h>

void get_date();
void set_date();
void get_time();
void set_time();
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
    } else if (strncasecmp(cmd, "help", 4) == 0) {
        help();
    } else if (strncasecmp(cmd, "x", 1) == 0) {
        debug();
    }
}

void debug() {
    RX8900SA_init(U5);
}

void get_date() {
    char date[11] = {0};

    RX8900SA_get_date(U5, date);

    printf(">>> DATE:%s\n", date);
}

void set_date() {
    uint16_t year = 2024;
    uint8_t month = 5;
    uint8_t day = 24;

    RX8900SA_set_date(U5, year, month, day);
}

void get_time() {
    char time[11] = {0};

    RX8900SA_get_time(U5, time);

    printf(">>> TIME:%s\n", time);
}

void set_time() {
    uint8_t hour = 12;
    uint8_t minute = 34;
    uint8_t second = 56;

    RX8900SA_set_time(U5, hour, minute, second);
}

void help() {
    printf("BREAKOUT Rev.0\n");
    printf("\n");
    printf("Commands:\n");
    printf("  get date\n");
    printf("  set date\n");
    printf("  get time\n");
    printf("  set time\n");
    printf("\n");
}