#include <stdio.h>
#include <string.h>

#include <cli.h>

void echo(const char *);

const uint8_t height = 25;

typedef struct CLI {
    // int32_t timer;
    int ix;
    char buffer[64];
} CLI;

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

        // Add character to buffer
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
