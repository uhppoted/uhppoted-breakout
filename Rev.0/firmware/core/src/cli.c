#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

#include <I2C0.h>
#include <I2C1.h>
#include <RTC.h>
#include <U2.h>
#include <U3.h>
#include <U4.h>
#include <cli.h>
#include <encoding/BER/BER.h>
#include <encoding/SSMP/SSMP.h>
#include <encoding/bisync/bisync.h>
#include <log.h>
#include <state.h>
#include <sys.h>

typedef struct CLI {
    int rows;
    int columns;
    char buffer[64];
    int ix;
    int32_t timer;
    int32_t ping;
} CLI;

const uint32_t CLI_TIMEOUT = 5000;     // ms
const uint32_t CLI_PING_TIMEOUT = 250; // ms
const uint8_t height = 25;

int64_t cli_timeout(alarm_id_t id, void *data);
int64_t cli_ping_timeout(alarm_id_t id, void *data);
void cli_on_terminal_report(const char *buffer, int N);

void echo(const char *line);
void clearline();
void cpr(char *cmd);
void display(const char *fmt, ...);
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

void state();
void scan();
void reboot();

void clear();
void help();
void debug();

CLI cli = {
    .rows = 40,
    .columns = 120,
    .buffer = {0},
    .ix = 0,
    .timer = -1,
};

extern const char *TERMINAL_CLEAR;
extern const char *TERMINAL_QUERY_CODE;
extern const char *TERMINAL_QUERY_STATUS;
extern const char *TERMINAL_QUERY_SIZE;
extern const char *TERMINAL_SET_SCROLL_AREA;
extern const char *TERMINAL_ECHO;
extern const char *TERMINAL_CLEARLINE;
extern const char *TERMINAL_DISPLAY;
extern const char *TERMINAL_AT;

const char CR = '\n';
const char LF = '\r';

const char *HELP[] = {
    "",
    "",
    "BREAKOUT Rev.0",
    "",
    "Commands:",
    "  get date",
    "  set date <YYYY-MM-DD>",
    "  get time",
    "  set time <HH:MM:SS>",
    "  get datetime",
    "  set datetime <YYYY-MM-DD HH:MM:SS>",
    "  get weekday",
    "",
    "  unlock door <1|2|3|4>",
    "  lock door <1|2|3|4>",
    "  set LED <1|2|3|4|SYS|IN|ERR>",
    "  clear LED <1|2|3|4|SYS|IN|ERR>",
    "",
    "  get doors",
    "  get buttons",
    "",
    "  state",
    "  scan",
    "  reboot",
    "",
    "  clear",
    "  help",
    ""};

/** Sets the scroll area.
 *
 */
void cli_init() {
    print(TERMINAL_CLEAR);
    print(TERMINAL_QUERY_CODE);
    print(TERMINAL_QUERY_SIZE);
    printf(TERMINAL_QUERY_STATUS);
}

/** Queries the terminal ID 'out of band'.
 *
 */
void cli_ping() {
    cli.ping = add_alarm_in_ms(CLI_PING_TIMEOUT, cli_ping_timeout, (CLI *)&cli, true);
    printf(TERMINAL_QUERY_STATUS);
}

/** Processes received characters.
 *
 */
void cli_rx(const struct buffer *received) {
    int N = received->N;

    // terminal message?
    if (N > 0 && received->data[0] == 27) {
        const char *data = received->data;
        int ix = 0;

        while (ix < N) {
            if (data[ix] == 27) {
                int jx = ix;
                while (++jx < N && data[jx] != 27) {
                }

                cli_on_terminal_report(&data[ix], jx - ix);
            }

            ix++;
        }

        return;
    }

    // ... typed characters presumably
    if (cli.timer > 0) {
        cancel_alarm(cli.timer);
        cli.timer = 0;
    }

    for (int i = 0; i < N; i++) {
        char ch = received->data[i];

        // SYN?
        if (ch == SYN) {
            memset(cli.buffer, 0, sizeof(cli.buffer));
            cli.ix = 0;
            continue;
        }

        // ENQ?
        if (ch == ENQ && cli.ix == 0) {
            memset(cli.buffer, 0, sizeof(cli.buffer));
            cli.ix = 0;
            set_mode(MODE_SSMP);
            break;
        }

        // ESC?
        if (ch == 27) {
            memset(cli.buffer, 0, sizeof(cli.buffer));
            cli.ix = 0;
            break; // NTS: really not expecting and ESC character
        }

        // CRLF ?
        if (ch == CR || ch == LF) {
            if (cli.ix > 0) {
                exec(cli.buffer);
            }

            memset(cli.buffer, 0, sizeof(cli.buffer));
            cli.ix = 0;
            continue;
        }

        // backspace?
        if (ch == 8) {
            if (cli.ix > 0) {
                cli.buffer[--cli.ix] = 0;
                echo(cli.buffer);
            }

            continue;
        }

        // append character to buffer
        if (cli.ix < (sizeof(cli.buffer) - 1)) {
            cli.buffer[cli.ix++] = ch;
            cli.buffer[cli.ix] = 0;

            echo(cli.buffer);
            continue;
        }
    }

    if (cli.ix > 0) {
        cli.timer = add_alarm_in_ms(CLI_TIMEOUT, cli_timeout, (CLI *)&cli, true);
    }
}

/** Processes ANSI/VT100 terminal report.
 *
 */
void cli_on_terminal_report(const char *data, int N) {
    // ... report device code ?
    // e.g. [27 91 53 55 59 50 48 50 82 27 91 63 49 59 50 99 ]
    if (N >= 5 && data[0] == 27 && data[1] == '[' && data[N - 1] == 'c') {
    }

    // ... report device status?
    if (N >= 4 && data[0] == 27 && data[1] == '[' && data[2] == '0' && data[3] == 'n') {
        set_mode(MODE_CLI);
        cancel_alarm(cli.ping);
    }

    // ... report cursor position (ESC[#;#R)
    // e.g. [27 91 53 55 59 50 48 50 82 ]
    if (N >= 6 && data[0] == 27 && data[1] == '[' && data[N - 1] == 'R') {
        char *code = strndup(&data[2], N - 3);

        cpr(code);
        free(code);
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

/* 'ping' timeout handler. Sets the system 'mode' to 'unknown'.
 *
 */
int64_t cli_ping_timeout(alarm_id_t id, void *data) {
    if (get_mode() == MODE_CLI) {
        set_mode(MODE_UNKNOWN);
    }

    return 0;
}

/* Clears the terminal and queries window size
 *
 */
void clear() {
    print(TERMINAL_CLEAR);
    print(TERMINAL_QUERY_SIZE);
}

/* Cursor position report.
 *
 *  Sets the scrolling window with space at the bottom for the command echo.
 */
void cpr(char *cmd) {
    int rows;
    int cols;
    int rc = sscanf(cmd, "%d;%d", &rows, &cols);

    if (rc > 0) {
        cli.rows = rows;
        cli.columns = cols;

        if (rows > 20) {
            int h = rows - 5;
            char s[24];

            snprintf(s, sizeof(s), TERMINAL_SET_SCROLL_AREA, h);
            print(s);
        }
    }
}

/* Saves the cursor position, displays the current command buffer and then restores
 *  the cursor position
 *
 */
void echo(const char *cmd) {
    char s[64];
    int h = cli.rows - 4;

    snprintf(s, sizeof(s), TERMINAL_ECHO, h, cmd);
    print(s);
}

/* Saves the cursor position, clears the command line, redisplays the prompt and then
 *   restores the cursor position.
 *
 */
void clearline() {
    int h = cli.rows - 4;
    char s[24];

    snprintf(s, sizeof(s), TERMINAL_CLEARLINE, h);
    print(s);
}

/* Saves the cursor position, displays the command output and then restores the cursor
 *   position
 *
 */
void display(const char *fmt, ...) {
    int y = cli.rows - 3;
    char text[256];
    char s[256];

    va_list args;

    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    snprintf(s, sizeof(s), TERMINAL_DISPLAY, y, text);
    print(s);
}

void exec(char *cmd) {
    char s[128];

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
    } else if (strncasecmp(cmd, "state", 5) == 0) {
        state();
    } else if (strncasecmp(cmd, "scan", 4) == 0) {
        scan();
    } else if (strncasecmp(cmd, "reboot", 6) == 0) {
        reboot();
    } else if (strncasecmp(cmd, "clear", 5) == 0) {
        clear();
    } else if (strncasecmp(cmd, "help", 4) == 0) {
        help();
    } else if (strncasecmp(cmd, "debug", 5) == 0) {
        debug();
    } else {
        display("unknown command (%s)\n", cmd);
    }
}

void debug() {
    packet p = {
        .tag = PACKET_GET_RESPONSE,
        .version = 0,
        .community = "public",
        .get_response = {
            .request_id = 1,
            .error = 0,
            .error_index = 0,
            .OID = "0.1.3.6.655136.1.1",
            .value = {
                .tag = VALUE_UINT32,
                .integer = 405419896,
            },
        },
    };

    // field fnull = {
    //     .tag = FIELD_NULL,
    //     .null = {},
    // };

    // field fint = {
    //     .tag = FIELD_INTEGER,
    //     .integer = {
    //         .value = 405419896,
    //         // .value = 9223372036854775807,
    //         // .value = -9223372036854775807 - 1,
    //         // .value = -1,
    //     },
    // };

    // field foid = {
    //     .tag = FIELD_OID,
    //     .OID = {
    //         .OID = "0.1.3.6.655136.1.1",
    //     },
    // };

    // field fcontent = {
    //     .tag = FIELD_SEQUENCE,
    //     .sequence = {
    //         .fields = vector_new(),
    //     },
    // };

    // fcontent.sequence.fields = vector_add(fcontent.sequence.fields, &foid);
    // fcontent.sequence.fields = vector_add(fcontent.sequence.fields, &fint);
    // // fcontent.sequence.fields = vector_add(fsequence.sequence.fields, &fnull);

    // field fseq = {
    //     .tag = FIELD_SEQUENCE,
    //     .sequence = {
    //         .fields = vector_new(),
    //     },
    // };

    // fseq.sequence.fields = vector_add(fseq.sequence.fields, &fcontent);

    // field fpdu = {
    //     .tag = FIELD_PDU_GET_RESPONSE,
    //     .get_response = {
    //         .fields = vector_new(),
    //     },
    // };

    // fpdu.sequence.fields = vector_add(fpdu.sequence.fields, &foid);
    // fpdu.sequence.fields = vector_add(fpdu.sequence.fields, &fint);

    // slice s = BER_encode(fpdu);

    slice s = ssmp_encode(p);

    printf("---\n");
    for (int i = 0; i < s.length; i++) {
        printf(" %02X", s.bytes[i]);
    };
    printf("\n---\n");

    slice_free(&s);

    // // clang-format off
    // const uint8_t msg[] = {
    //     48, 37, 2, 1, 0, 4, 6, 112, 117, 98, 108, 105, 99,
    //     160, 24,
    //          2, 1, 1,
    //          2, 1, 0,
    //          2, 1, 0,
    //          48, 13, 48, 11, 6, 7, 43, 6, 167, 254, 32, 1, 1, 5, 0,
    //
    //     // 48, 38, 2, 2, 0, 13, 4, 6, 112, 117, 98, 108, 105, 99,
    //     // 160, 24, 2, 1, 1, 2, 1, 0, 2,
    //     // 1, 0, 48, 13, 48, 11, 6, 7, 43, 6, 167, 254, 32,
    //     // 1, 1, 5, 0,
    // };
    // // clang-format on
    //
    // // printf("\n");
    // // for (int i = 0; i < 39; i++) {
    // //     printf("%02X", packet[i]);
    // // }
    // // printf("\n");
    //
    // packet *p = BER_decode(msg, sizeof(msg));
    //
    // debugf("CLI", ">>> PDU/GET");
    // debugf("CLI", ">>> PDU/GET version     %lld", p->get.version);
    // debugf("CLI", ">>> PDU/GET community   %s", p->get.community);
    // debugf("CLI", ">>> PDU/GET request ID  %lld", p->get.request_id);
    // debugf("CLI", ">>> PDU/GET error       %lld", p->get.error);
    // debugf("CLI", ">>> PDU/GET error index %lld", p->get.error_index);
    // debugf("CLI", ">>> PDU/GET OID         %s", p->get.OID);
    // debugf("CLI", ">>> PDU/GET value       null");
    //
    // packet_free(p);
}

void state() {
    debugf("CLI", ">>> I2C   %s\n", (get_error(ERR_I2C_GENERIC) || get_error(ERR_I2C_TIMEOUT)) ? "error" : "ok");
    debugf("CLI", ">>> queue %s\n", get_error(ERR_QUEUE_FULL) ? "error" : "ok");
    debugf("CLI", ">>> RTC   %s\n", get_error(ERR_RX8900SA) ? "error" : "ok");
    debugf("CLI", ">>> U3    %s\n", get_error(ERR_U3) ? "error" : "ok");
    debugf("CLI", ">>> U4    %s\n", get_error(ERR_U4) ? "error" : "ok");
    debugf("CLI", ">>> other %s\n", get_error(ERR_UNKNOWN) ? "error" : "ok");
}

void get_datetime() {
    char date[11] = {0};
    char time[11] = {0};

    RTC_get_date(date, 11);
    RTC_get_time(time, 11);

    display("get-datetime: %s %s\n", date, time);
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
        display("set-datetime: ok");
        return;
    }

    if ((rc = sscanf(cmd, "%04d-%02d-%02d %02d:%02d", &year, &month, &day, &hour, &minute)) == 5) {
        RTC_set_date(year, month, day);
        RTC_set_time(hour, minute, 0);
        display("set-datetime: ok");
        return;
    }

    display("*** INVALID DATE/TIME (%s)", cmd);
}

void get_date() {
    char date[11] = {0};

    RTC_get_date(date, 11);

    display("get-date: %s", date);
}

void set_date(const char *cmd) {
    int year;
    int month;
    int day;
    int rc;

    if ((rc = sscanf(cmd, "%04d-%02d-%02d", &year, &month, &day)) == 3) {
        RTC_set_date(year, month, day);
        display("set-date: ok");
        return;
    }

    display("*** INVALID DATE (%s)", cmd);
}

void get_time() {
    char time[11] = {0};

    RTC_get_time(time, 11);

    display("get-time: %s", time);
}

void set_time(const char *cmd) {
    int hour;
    int minute;
    int second;
    int rc;

    if ((rc = sscanf(cmd, "%02d:%02d:%02d", &hour, &minute, &second)) == 3) {
        RTC_set_time(hour, minute, second);
        display("set-time: ok");
        return;
    }

    if ((rc = sscanf(cmd, "%02d:%02d", &hour, &minute)) == 2) {
        RTC_set_time(hour, minute, 0);
        display("set-time: ok");
        return;
    }

    display("*** INVALID TIME (%s)", cmd);
}

void get_weekday() {
    char weekday[10] = {0};

    RTC_get_dow(weekday, 10);

    display("get-weekday: %s", weekday);
}

void unlock_door(const char *cmd) {
    int N = strlen(cmd);

    if (N > 0) {
        uint relay;
        int rc;

        if ((rc = sscanf(cmd, "%u", &relay)) == 1) {
            U4_set_relay(relay, 5000);
            display("unlock door: ok");
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
            display("lock-door: ok");
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
            display("set-LED: ok");
            return;
        }

        if (strncasecmp(cmd, "ERR", 3) == 0) {
            if (state) {
                U4_set_ERR();
            } else {
                U4_clear_ERR();
            }
            display("set-ERR: ok");
            return;
        }

        if (strncasecmp(cmd, "IN", 2) == 0) {
            if (state) {
                U4_set_IN();
            } else {
                U4_clear_IN();
            }
            display("set-IN: ok");
            return;
        }

        if (strncasecmp(cmd, "SYS", 3) == 0) {
            if (state) {
                U4_set_SYS();
            } else {
                U4_clear_SYS();
            }
            display("set-SYS: ok");
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
            display("blink-LED: ok");
            return;
        }

        if (strncasecmp(cmd, "ERR", 3) == 0) {
            U4_blink_ERR(5, 500);
            display("blink-ERR: ok");
            return;
        }

        if (strncasecmp(cmd, "IN", 2) == 0) {
            U4_blink_IN(5, 500);
            display("blink-IN: ok");
            return;
        }

        if (strncasecmp(cmd, "SYS", 3) == 0) {
            U4_blink_SYS(5, 500);
            display("blink-SYS: ok");
            return;
        }
    }
}

void get_doors() {
    for (uint8_t door = 1; door <= 4; door++) {
        bool open = U3_get_door(door);
        display("door %u %s", door, open ? "open" : "closed");
    };
}

void get_buttons() {
    for (uint8_t door = 1; door <= 4; door++) {
        bool pressed = U3_get_button(door);
        display("button %u %s", door, pressed ? "pressed" : "released");
    };
}

void scan() {
    I2C0_scan();
    I2C1_scan();
}

/* Tight loop until watchdog reboots the system.
 *
 */
void reboot() {
    display("... rebooting ... ");
    sys_reboot();
}

void help() {
    int N = sizeof(HELP) / sizeof(char *);
    int X = cli.columns - 48;
    int Y = 0;
    char s[128];

    for (int i = 0; i < N; i++) {
        snprintf(s, sizeof(s), TERMINAL_AT, Y + i, X, HELP[i]);
        print(s);
    }
}
