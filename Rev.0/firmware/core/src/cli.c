#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <I2C0.h>
#include <I2C1.h>
#include <RTC.h>
#include <U2.h>
#include <U3.h>
#include <U4.h>
#include <cli.h>
#include <log.h>

typedef struct CLI {
    // int32_t timer;
    int ix;
    char buffer[64];
} CLI;

const uint8_t height = 25;

void echo(const char *line);
void exec(char *cmd);
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

/** Processes received UART characters.
 *
 */
void rx(char *received) {
    static CLI cli = {
        // .timer = -1,
        .ix = 0,
        .buffer = {0},
    };

    int N = strlen(received);
    for (int i = 0; i < N; i++) {
        char ch = received[i];

        // CRLF ?
        if (ch == '\n' || ch == '\r') {
            if (cli.ix > 0) {
                exec(cli.buffer);
            }

            memset(cli.buffer, 0, sizeof(cli.buffer));
            cli.ix = 0;
            continue;
        }

        // // VT100 escape code?
        // if (cli.buffer[0] == 27 && ch == 'R' && (cli.ix < sizeof(cli.buffer) - 1)) { // VT100 cursor position report
        //     cli.buffer[cli.ix++] = ch;
        //     cli.buffer[cli.ix] = 0;
        //
        //     cpr(&cli.buffer[1]);
        //     memset(cli.buffer, 0, sizeof(cli.buffer));
        //     cli.ix = 0;
        //     continue;
        // }

        // backspace?
        if (ch == 8) {
            if (cli.ix > 0) {
                cli.buffer[--cli.ix] = 0;
                echo(cli.buffer);
            }

            // if (cli.ix > 0) {
            //     cli.timer = add_alarm_in_ms(CLI_TIMEOUT, cli_timeout, (CLI *)&cli, true);
            // }

            continue;
        }

        // append character to buffer
        if (cli.ix < (sizeof(cli.buffer) - 1)) {
            cli.buffer[cli.ix++] = ch;
            cli.buffer[cli.ix] = 0;

            // ... echo if normal commnad and not a VT100 code
            if (cli.buffer[0] != 27) {
                echo(cli.buffer);
            }

            // cli.timer = add_alarm_in_ms(CLI_TIMEOUT, cli_timeout, (CLI *)&cli, true);
            continue;
        }
    }
}

/* Saves the cursor position, displays the current command buffer and then restores
 *  the cursor position
 *
 */
void echo(const char *cmd) {
    char s[64];
    // snprintf(s, sizeof(s), "\0337\033[%d;0H>> %s\033[0K\0338", height, cmd);
    snprintf(s, sizeof(s), "\r>> %s\033[0K", cmd);
    fputs(s, stdout);
    fflush(stdout);
}

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
        printf(">>>> ???? %s\n", cmd);
    }
}

void debug() {
}

void reset() {
    RTC_reset();
}

void get_date() {
    char date[11] = {0};

    RTC_get_date(date, 11);

    printf(">>> %s\n", date);
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

    printf(">>> %s\n", time);
}

void get_weekday() {
    char weekday[9] = {0};

    RTC_get_dow(weekday, 9);

    printf(">>> %s\n", weekday);
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
        uint relay;
        int rc;

        if ((rc = sscanf(cmd, "%u", &relay)) == 1) {
            U4_set_relay(relay, state);
            printf("ok\n");
        }
    }
}

void set_LED(const char *cmd, bool state) {
    int N = strlen(cmd);

    if (N > 0) {
        uint LED;
        int rc;

        if ((rc = sscanf(cmd, "%u", &LED)) == 1) {
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
    printf("  get time\n");
    printf("  get weekday\n");
    printf("  set date\n");
    printf("  set time\n");
    printf("\n");
    printf("  set relay <1|2|3|4>\n");
    printf("  clear relay <1|2|3|4>\n");
    printf("  set LED <1|2|3|4|SYS|IN|ERR>\n");
    printf("  clear LED <1|2|3|4|SYS|IN|ERR>\n");
    printf("\n");
    printf("  reset\n");
    printf("  scan\n");
    printf("\n");
}