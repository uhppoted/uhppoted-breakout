#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void debugf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];

    snprintf(format, sizeof(format), "%-5s  %-10s %s\n", "DEBUG", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vprintf(format, args);
    va_end(args);
}

void infof(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];

    snprintf(format, sizeof(format), "%-5s  %-10s %s\n", "INFO", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vprintf(format, args);
    va_end(args);
}

void warnf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];

    snprintf(format, sizeof(format), "%-5s  %-10s %s\n", "WARN", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vprintf(format, args);
    va_end(args);
}

void errorf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];

    snprintf(format, sizeof(format), "%-5s  %-10s %s\n", "ERROR", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vprintf(format, args);
    va_end(args);
}
