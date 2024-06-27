#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/util/queue.h>

struct {
    queue_t queue;
    repeating_timer_t timer;
} LOG = {

};

const int32_t LOG_TICK = 100;

bool log_tick(repeating_timer_t *rt);

void log_init() {
    queue_init(&LOG.queue, sizeof(char *), 64);

    add_repeating_timer_ms(LOG_TICK, log_tick, NULL, &LOG.timer);
}

/*
 * Outputs any pending log queue entries.
 */
bool log_tick(repeating_timer_t *rt) {
    char *msg = NULL;
    if (queue_try_remove(&LOG.queue, &msg)) {
        if (msg != NULL) {
            printf("%s\n", msg);
            free(msg);
        }
    }

    return true;
}

void debugf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char *msg = (char *)calloc(64, sizeof(char));

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "DEBUG", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 64, format, args);
    va_end(args);

    if (queue_is_full(&LOG.queue) || !queue_try_add(&LOG.queue, &msg)) {
        free(msg);
    }
}

void infof(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char *msg = (char *)calloc(64, sizeof(char));

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "INFO", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 64, format, args);
    va_end(args);

    if (queue_is_full(&LOG.queue) || !queue_try_add(&LOG.queue, &msg)) {
        free(msg);
    }
}

void warnf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char *msg = (char *)calloc(64, sizeof(char));

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "WARN", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 64, format, args);
    va_end(args);

    if (queue_is_full(&LOG.queue) || !queue_try_add(&LOG.queue, &msg)) {
        free(msg);
    }
}

void errorf(const char *tag, const char *fmt, ...) {
    int N = strlen(fmt);
    char format[N + 32];
    char *msg = (char *)calloc(64, sizeof(char));

    snprintf(format, sizeof(format), "%-5s  %-10s %s", "ERROR", tag, fmt);

    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 64, format, args);
    va_end(args);

    if (queue_is_full(&LOG.queue) || !queue_try_add(&LOG.queue, &msg)) {
        free(msg);
    }
}
