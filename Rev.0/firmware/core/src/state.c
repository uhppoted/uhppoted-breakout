#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <breakout.h>
#include <log.h>

void set_error(err error, const char *tag, const char *fmt, ...) {
    switch (error) {
    case ERR_OK:
        break;

    case ERR_I2C_GENERIC:
        break;

    case ERR_I2C_TIMEOUT:
        break;

    case ERR_QUEUE_FULL:
        break;

    case ERR_RX8900SA:
        break;

    case ERR_UNKNOWN:
        break;
    }

    int N = strlen(fmt);
    char format[N + 32];

    snprintf(format, sizeof(format), "%-5s  %-10s %s\n", "ERROR", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vprintf(format, args);
    va_end(args);
}