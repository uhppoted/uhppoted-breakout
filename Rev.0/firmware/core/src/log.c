#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/util/queue.h>

#include <sys.h>

void log_init() {
}

void debugf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char msg[128];

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "DEBUG", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    println(msg);
}

void infof(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char msg[128];

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "INFO", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    println(msg);
}

void warnf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char msg[128];

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "WARN", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    println(msg);
}

void errorf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char msg[128];

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "ERROR", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    println(msg);
}
