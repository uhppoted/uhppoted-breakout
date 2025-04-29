#include <stdarg.h>
#include <stdio.h>

#include <breakout.h>
#include <log.h>

#define LOGTAG "SYS"

extern queue_t queue;

typedef struct _err {
    bool value;
    uint16_t logged;
    uint16_t logcount;
} _err;

typedef struct _state {
    struct {
        _err queue;
        _err memory;
        _err I2C;
        _err RX8900SA;
        _err U2;
        _err U3;
        _err U4;
        _err watchdog;
        _err debug;
        _err unknown;
    } errors;
} _state;

_state STATE = {
    .errors = {
        .queue = {
            .value = false,
            .logged = 0,
            .logcount = 25,
        },
        .memory = {
            .value = false,
            .logged = 0,
            .logcount = 25,
        },
        .I2C = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
        .RX8900SA = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
        .U2 = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
        .U3 = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
        .U4 = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
        .watchdog = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
        .debug = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
        .unknown = {
            .value = false,
            .logged = 0,
            .logcount = 1,
        },
    },
};

const uint16_t BITMASK_ERR_MEMORY = 0x0001;
const uint16_t BITMASK_ERR_QUEUE = 0x0002;
const uint16_t BITMASK_ERR_I2C = 0x0004;
const uint16_t BITMASK_ERR_RX8900SA = 0x0008;
const uint16_t BITMASK_ERR_U2 = 0x0010;
const uint16_t BITMASK_ERR_U3 = 0x0020;
const uint16_t BITMASK_ERR_U4 = 0x0040;
const uint16_t BITMASK_ERR_WATCHDOG = 0x0080;
const uint16_t BITMASK_ERR_DEBUG = 0x4000;
const uint16_t BITMASK_ERR_UNKNOWN = 0x8000;

// NTS: SRAM_BASE is 0x20000000
bool push(message msg) {
    uint32_t m = msg.message;

    switch (msg.tag) {
    case MESSAGE_NONE:
        m |= msg.none & 0x0fffffff;
        break;

    case MESSAGE_UINT32:
        m |= msg.u32 & 0x0fffffff;
        break;

    case MESSAGE_BUFFER:
        m |= ((uint32_t)msg.buffer & 0x0fffffff);
        break;

    case MESSAGE_SWIPE:
        m |= ((uint32_t)msg.swipe & 0x0fffffff);
        break;

    case MESSAGE_INPUTS:
        m |= msg.inputs & 0x0fffffff;
        break;
    }

    if (queue_is_full(&queue) || !queue_try_add(&queue, &m)) {
        set_error(ERR_QUEUE_FULL, LOGTAG, "queue full");
        return false;
    }

    return true;
}

_err *_find(err error) {
    switch (error) {
    case ERR_OK:
        return NULL;

    case ERR_I2C_GENERIC:
        return &STATE.errors.I2C;

    case ERR_I2C_TIMEOUT:
        return &STATE.errors.I2C;

    case ERR_QUEUE_FULL:
        return &STATE.errors.queue;

    case ERR_MEMORY:
        return &STATE.errors.memory;

    case ERR_RX8900SA:
        return &STATE.errors.RX8900SA;

    case ERR_U2:
        return &STATE.errors.U2;

    case ERR_U3:
        return &STATE.errors.U3;

    case ERR_U4:
        return &STATE.errors.U4;

    case ERR_WATCHDOG:
        return &STATE.errors.watchdog;

    case ERR_DEBUG:
        return &STATE.errors.debug;
        break;

    case ERR_UNKNOWN:
        return &STATE.errors.unknown;
    }

    return NULL;
}

void set_error(err error, const char *tag, const char *fmt, ...) {
    _err *e = _find(error);

    if (e != NULL) {
        bool loggable = e->logged == 0;
        e->value = true;
        e->logged = (e->logged + 1) % e->logcount;

        if (loggable) {
            char msg[64];

            va_list args;
            va_start(args, fmt);
            vsnprintf(msg, sizeof(msg), fmt, args);
            va_end(args);

            errorf(tag, "%s", msg);
        }
    }
}

bool get_error(err error) {
    switch (error) {
    case ERR_I2C_GENERIC:
        return STATE.errors.I2C.value;

    case ERR_I2C_TIMEOUT:
        return STATE.errors.I2C.value;

    case ERR_QUEUE_FULL:
        return STATE.errors.queue.value;

    case ERR_MEMORY:
        return STATE.errors.memory.value;

    case ERR_RX8900SA:
        return STATE.errors.RX8900SA.value;

    case ERR_U2:
        return STATE.errors.U2.value;

    case ERR_U3:
        return STATE.errors.U3.value;

    case ERR_U4:
        return STATE.errors.U4.value;

    case ERR_WATCHDOG:
        return STATE.errors.watchdog.value;

    case ERR_DEBUG:
        return STATE.errors.debug.value;

    case ERR_UNKNOWN:
        return STATE.errors.unknown.value;
    }

    return false;
}

uint16_t get_errors() {
    uint16_t bits = 0x0000;

    bits |= STATE.errors.memory.value ? BITMASK_ERR_MEMORY : 0x0000;
    bits |= STATE.errors.queue.value ? BITMASK_ERR_QUEUE : 0x0000;
    bits |= STATE.errors.I2C.value ? BITMASK_ERR_I2C : 0x0000;
    bits |= STATE.errors.RX8900SA.value ? BITMASK_ERR_RX8900SA : 0x0000;
    bits |= STATE.errors.U2.value ? BITMASK_ERR_U2 : 0x0000;
    bits |= STATE.errors.U3.value ? BITMASK_ERR_U3 : 0x0000;
    bits |= STATE.errors.U4.value ? BITMASK_ERR_U4 : 0x0000;
    bits |= STATE.errors.watchdog.value ? BITMASK_ERR_WATCHDOG : 0x0000;
    bits |= STATE.errors.debug.value ? BITMASK_ERR_DEBUG : 0x0000;
    bits |= STATE.errors.unknown.value ? BITMASK_ERR_UNKNOWN : 0x0000;

    //  STATE.errors.memory.value = false;
    STATE.errors.I2C.value = false;
    STATE.errors.queue.value = false;
    STATE.errors.RX8900SA.value = false;
    STATE.errors.U2.value = false;
    STATE.errors.U3.value = false;
    STATE.errors.U4.value = false;
    //  STATE.errors.watchdog.value = false;
    //  STATE.errors.debug.value = false;
    STATE.errors.unknown.value = false;

    return bits;
}