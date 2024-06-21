#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include <I2C0.h>
#include <I2C1.h>
#include <RTC.h>
#include <U2.h>
#include <U3.h>
#include <U4.h>
#include <cli.h>
#include <log.h>
#include <txrx.h>

typedef struct CLI {
    int32_t timer;
    int ix;
    char buffer[64];
} CLI;

const uint32_t CLI_TIMEOUT = 10000; // ms
const uint8_t height = 25;

int64_t cli_timeout(alarm_id_t id, void *data);
void echo(const char *line);
void clearline();
void exec(char *cmd);

void get_datetime();
void set_datetime(const char *cmd);
void get_date();
void set_date(const char *cmd);
void get_time();
void set_time(const char *cmd);
void get_weekday();

void unlock_door(const char *door);
void lock_door(const char *door);
void set_LED(const char *led, bool state);
void blink_LED(const char *led);
void get_doors();
void get_buttons();

void reset();
void scan();

void clear();
void help();
void debug();

/** Processes received characters.
 *
 */
void rx(char *received) {
    static CLI cli = {
        .timer = -1,
        .ix = 0,
        .buffer = {0},
    };

    if (cli.timer > 0) {
        cancel_alarm(cli.timer);
        cli.timer = 0;
    }

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

            if (cli.ix > 0) {
                cli.timer = add_alarm_in_ms(CLI_TIMEOUT, cli_timeout, (CLI *)&cli, true);
            }

            continue;
        }

        // append character to buffer
        if (cli.ix < (sizeof(cli.buffer) - 1)) {
            cli.buffer[cli.ix++] = ch;
            cli.buffer[cli.ix] = 0;

            // ... echo if normal command and not a VT100 code
            if (cli.buffer[0] != 27) {
                echo(cli.buffer);
            }

            cli.timer = add_alarm_in_ms(CLI_TIMEOUT, cli_timeout, (CLI *)&cli, true);
            continue;
        }
    }
}

/* Timeout handler. Clears the current command and command line.
 *
 */
int64_t cli_timeout(alarm_id_t id, void *data) {
    CLI *cli = (CLI *)data;
    memset(cli->buffer, 0, sizeof(cli->buffer));
    cli->ix = 0;
    cli->timer = 0;

    clearline();

    return 0;
}

/* Redisplays the current command buffer and clears trailing characters.
 *
 */
void echo(const char *cmd) {
    char s[64];
    snprintf(s, sizeof(s), "\r>> %s\033[0K", cmd);
    fputs(s, stdout);
    fflush(stdout);
}

/* Saves the cursor position, clears the command line, redisplays the prompt and then
 * restores the cursor position.
 *
 */
void clearline() {
    //     char s[24];
    //     snprintf(s, sizeof(s), "\0337\033[%d;0H>> \033[0K\0338", height);
    //     fputs(s, stdout);
    //     fflush(stdout);
    //
    echo("");
}

void exec(char *cmd) {
    char s[128];

    printf("\n");

    if (strncasecmp(cmd, "get datetime", 12) == 0) {
        get_datetime();
    } else if (strncasecmp(cmd, "set datetime ", 13) == 0) {
        set_datetime(&cmd[13]);
    } else if (strncasecmp(cmd, "get date", 8) == 0) {
        get_date();
    } else if (strncasecmp(cmd, "set date ", 9) == 0) {
        set_date(&cmd[9]);
    } else if (strncasecmp(cmd, "get time", 8) == 0) {
        get_time();
    } else if (strncasecmp(cmd, "set time ", 9) == 0) {
        set_time(&cmd[9]);
    } else if (strncasecmp(cmd, "get weekday", 11) == 0) {
        get_weekday();
    } else if (strncasecmp(cmd, "unlock door ", 12) == 0) {
        unlock_door(&cmd[12]);
    } else if (strncasecmp(cmd, "lock door ", 10) == 0) {
        lock_door(&cmd[10]);
    } else if (strncasecmp(cmd, "set LED ", 8) == 0) {
        set_LED(&cmd[8], true);
    } else if (strncasecmp(cmd, "clear LED ", 10) == 0) {
        set_LED(&cmd[10], false);
    } else if (strncasecmp(cmd, "blink LED ", 10) == 0) {
        blink_LED(&cmd[10]);
    } else if (strncasecmp(cmd, "get doors", 9) == 0) {
        get_doors();
    } else if (strncasecmp(cmd, "get buttons", 11) == 0) {
        get_buttons();
    } else if (strncasecmp(cmd, "reset", 5) == 0) {
        reset();
    } else if (strncasecmp(cmd, "scan", 4) == 0) {
        scan();
    } else if (strncasecmp(cmd, "clear", 5) == 0) {
        clear();
    } else if (strncasecmp(cmd, "help", 4) == 0) {
        help();
    } else if (strncasecmp(cmd, "debug", 5) == 0) {
        debug();
    } else {
        printf(">>>> ???? %s\n", cmd);
    }
}

void debug() {
    TXRX_debug();
}

void reset() {
    RTC_reset();
}

void get_datetime() {
    char date[11] = {0};
    char time[11] = {0};

    RTC_get_date(date, 11);
    RTC_get_time(time, 11);

    printf(">>> %s %s\n", date, time);
}

void set_datetime(const char *cmd) {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int rc;

    if ((rc = sscanf(cmd, "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second)) == 6) {
        RTC_set_date(year, month, day);
        RTC_set_time(hour, minute, second);
        printf("ok\n");
        return;
    }

    if ((rc = sscanf(cmd, "%04d-%02d-%02d %02d:%02d", &year, &month, &day, &hour, &minute)) == 5) {
        RTC_set_date(year, month, day);
        RTC_set_time(hour, minute, 0);
        printf("ok\n");
        return;
    }

    printf(">> invalid date (%s)\n", cmd);
}

void get_date() {
    char date[11] = {0};

    RTC_get_date(date, 11);

    printf(">>> %s\n", date);
}

void set_date(const char *cmd) {
    int year;
    int month;
    int day;
    int rc;

    if ((rc = sscanf(cmd, "%04d-%02d-%02d", &year, &month, &day)) == 3) {
        RTC_set_date(year, month, day);
        printf("ok\n");
        return;
    }

    printf(">> invalid date (%s)\n", cmd);
}

void get_time() {
    char time[11] = {0};

    RTC_get_time(time, 11);

    printf(">>> %s\n", time);
}

void set_time(const char *cmd) {
    int hour;
    int minute;
    int second;
    int rc;

    if ((rc = sscanf(cmd, "%02d:%02d:%02d", &hour, &minute, &second)) == 3) {
        RTC_set_time(hour, minute, second);
        printf("ok\n");
        return;
    }

    if ((rc = sscanf(cmd, "%02d:%02d", &hour, &minute)) == 2) {
        RTC_set_time(hour, minute, 0);
        printf("ok\n");
        return;
    }

    printf(">> invalid time (%s)\n", cmd);
}

void get_weekday() {
    char weekday[10] = {0};

    RTC_get_dow(weekday, 10);

    printf(">>> %s\n", weekday);
}

void unlock_door(const char *cmd) {
    int N = strlen(cmd);

    if (N > 0) {
        uint relay;
        int rc;

        if ((rc = sscanf(cmd, "%u", &relay)) == 1) {
            U4_set_relay(relay, 5000);
            printf("ok\n");
        }
    }
}

void lock_door(const char *cmd) {
    int N = strlen(cmd);

    if (N > 0) {
        uint relay;
        int rc;

        if ((rc = sscanf(cmd, "%u", &relay)) == 1) {
            U4_clear_relay(relay);
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
            if (state) {
                U4_set_LED(LED);

            } else {
                U4_clear_LED(LED);
            }
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

void blink_LED(const char *cmd) {
    int N = strlen(cmd);

    if (N > 0) {
        uint LED;
        int rc;

        if ((rc = sscanf(cmd, "%u", &LED)) == 1) {
            U4_blink_LED(LED, 5, 500);
            printf("ok\n");
            return;
        }
    }
}

void get_doors() {
    for (uint8_t door = 1; door <= 4; door++) {
        bool open = U3_get_door(door);
        printf(">>> door %u %s\n", door, open ? "open" : "closed");
    };
}

void get_buttons() {
    for (uint8_t door = 1; door <= 4; door++) {
        bool pressed = U3_get_button(door);
        printf(">>> button %u %s\n", door, pressed ? "pressed" : "released");
    };
}

void scan() {
    I2C0_scan();
    I2C1_scan();
}

/* Clears the terminal.
 *
 */
void clear() {
    fputs("\033c\033[2J", stdout);
    fflush(stdout);

    // set_scroll_area();
}

void help() {
    printf("BREAKOUT Rev.0\n");
    printf("\n");
    printf("Commands:\n");
    printf("  get date\n");
    printf("  set date <YYYY-MM-DD>\n");
    printf("  get time\n");
    printf("  set time <HH:MM:SS>\n");
    printf("  get datetime\n");
    printf("  set datetime <YYYY-MM-DD HH:MM:SS>\n");
    printf("  get weekday\n");
    printf("\n");
    printf("  unlock door <1|2|3|4>\n");
    printf("  lock door <1|2|3|4>\n");
    printf("  set LED <1|2|3|4|SYS|IN|ERR>\n");
    printf("  clear LED <1|2|3|4|SYS|IN|ERR>\n");
    printf("\n");
    printf("  get doors\n");
    printf("  get buttons\n");
    printf("\n");
    printf("  reset\n");
    printf("  scan\n");
    printf("\n");
    printf("  clear\n");
    printf("  help\n");
    printf("\n");
}
