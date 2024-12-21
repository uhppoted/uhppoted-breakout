#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <log.h>
#include <state.h>

struct {
    struct {
        bool queue;
        bool memory;
        bool I2C;
        bool RX8900SA;
        bool U3;
        bool U4;
        bool watchdog;
        bool debug;
        bool unknown;
    } errors;
} STATE = {
    .errors = {
        .queue = false,
        .memory = false,
        .I2C = false,
        .RX8900SA = false,
        .U3 = false,
        .U4 = false,
        .watchdog = false,
        .debug = false,
        .unknown = false,
    },
};

const uint16_t BITMASK_ERR_MEMORY = 0x0001;
const uint16_t BITMASK_ERR_QUEUE = 0x0002;
const uint16_t BITMASK_ERR_I2C = 0x0004;
const uint16_t BITMASK_ERR_RX8900SA = 0x0008;
const uint16_t BITMASK_ERR_U3 = 0x0010;
const uint16_t BITMASK_ERR_U4 = 0x0020;
const uint16_t BITMASK_ERR_WATCHDOG = 0x0040;
const uint16_t BITMASK_ERR_DEBUG = 0x4000;
const uint16_t BITMASK_ERR_UNKNOWN = 0x8000;

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

    case ERR_MEMORY:
        STATE.errors.memory = true;
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

    case ERR_DEBUG:
        STATE.errors.debug = true;
        break;

    case ERR_UNKNOWN:
        break;
    }

    char msg[64];

    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    errorf(tag, "%s", msg);
}

bool get_error(err error) {
    switch (error) {
    case ERR_I2C_GENERIC:
        return STATE.errors.I2C;

    case ERR_I2C_TIMEOUT:
        return STATE.errors.I2C;

    case ERR_QUEUE_FULL:
        return STATE.errors.queue;

    case ERR_MEMORY:
        return STATE.errors.memory;

    case ERR_RX8900SA:
        return STATE.errors.RX8900SA;

    case ERR_U3:
        return STATE.errors.U3;

    case ERR_U4:
        return STATE.errors.U4;

    case ERR_WATCHDOG:
        return STATE.errors.watchdog;

    case ERR_DEBUG:
        return STATE.errors.debug;

    case ERR_UNKNOWN:
        return STATE.errors.unknown;
    }

    return false;
}

uint16_t get_errors() {
    uint16_t bits = 0x0000;

    bits |= STATE.errors.memory ? BITMASK_ERR_MEMORY : 0x0000;
    bits |= STATE.errors.queue ? BITMASK_ERR_QUEUE : 0x0000;
    bits |= STATE.errors.I2C ? BITMASK_ERR_I2C : 0x0000;
    bits |= STATE.errors.RX8900SA ? BITMASK_ERR_RX8900SA : 0x0000;
    bits |= STATE.errors.U3 ? BITMASK_ERR_U3 : 0x0000;
    bits |= STATE.errors.U4 ? BITMASK_ERR_U4 : 0x0000;
    bits |= STATE.errors.watchdog ? BITMASK_ERR_WATCHDOG : 0x0000;
    bits |= STATE.errors.debug ? BITMASK_ERR_DEBUG : 0x0000;
    bits |= STATE.errors.unknown ? BITMASK_ERR_UNKNOWN : 0x0000;

    //  STATE.errors.memory = false;
    STATE.errors.I2C = false;
    STATE.errors.queue = false;
    STATE.errors.RX8900SA = false;
    STATE.errors.U3 = false;
    STATE.errors.U4 = false;
    //  STATE.errors.watchdog = false;
    //  STATE.errors.debug = false;
    STATE.errors.unknown = false;

    return bits;
}