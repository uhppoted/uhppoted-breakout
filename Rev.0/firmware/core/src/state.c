#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <log.h>
#include <state.h>

struct {
    struct {
        bool queue;
        bool I2C;
        bool RX8900SA;
        bool U3;
        bool U4;
        bool watchdog;
        bool unknown;
    } errors;
} STATE = {
    .errors = {
        .queue = false,
        .I2C = false,
        .RX8900SA = false,
        .U3 = false,
        .U4 = false,
        .watchdog = false,
        .unknown = false,
    },
};

void set_error(err error, const char *tag, const char *fmt, ...) {
    switch (error) {
    case ERR_OK:
        break;

    case ERR_I2C_GENERIC:
        STATE.errors.I2C = true;
        break;

    case ERR_I2C_TIMEOUT:
        STATE.errors.I2C = true;
        break;

    case ERR_QUEUE_FULL:
        STATE.errors.queue = true;
        break;

    case ERR_RX8900SA:
        STATE.errors.RX8900SA = true;
        break;

    case ERR_U3:
        STATE.errors.U3 = true;
        break;

    case ERR_U4:
        STATE.errors.U4 = true;
        break;

    case ERR_WATCHDOG:
        STATE.errors.watchdog = true;
        break;

    case ERR_UNKNOWN:
        break;
    }

    // char msg[64];
    //
    // va_list args;
    // va_start(args, fmt);
    // vsnprintf(msg, sizeof(msg), fmt, args);
    // va_end(args);
    //
    // errorf(tag, "%s", msg);
}

bool get_error(err error) {
    switch (error) {
    case ERR_I2C_GENERIC:
        return STATE.errors.I2C;

    case ERR_I2C_TIMEOUT:
        return STATE.errors.I2C;

    case ERR_QUEUE_FULL:
        return STATE.errors.queue;

    case ERR_RX8900SA:
        return STATE.errors.RX8900SA;

    case ERR_U3:
        return STATE.errors.U3;

    case ERR_U4:
        return STATE.errors.U4;

    case ERR_WATCHDOG:
        return STATE.errors.watchdog;

    case ERR_UNKNOWN:
        return STATE.errors.unknown;
    }

    return false;
}

uint16_t get_errors() {
    uint16_t bits = 0x0000;

    bits |= STATE.errors.I2C ? 0x0001 : 0x0000;
    bits |= STATE.errors.queue ? 0x0002 : 0x0000;
    bits |= STATE.errors.RX8900SA ? 0x0004 : 0x0000;
    bits |= STATE.errors.U3 ? 0x0008 : 0x0000;
    bits |= STATE.errors.U4 ? 0x0010 : 0x0000;
    bits |= STATE.errors.watchdog ? 0x0020 : 0x0000;
    bits |= STATE.errors.unknown ? 0x8000 : 0x0000;

    STATE.errors.I2C = false;
    STATE.errors.queue = false;
    STATE.errors.RX8900SA = false;
    STATE.errors.U3 = false;
    STATE.errors.U4 = false;
    //  STATE.errors.watchdog = false;
    STATE.errors.unknown = false;

    return bits;
}