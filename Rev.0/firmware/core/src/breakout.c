#include <stdarg.h>
#include <stdio.h>

#include <breakout.h>
#include <log.h>

#define LOGTAG "SYS"

extern queue_t queue;

typedef struct _err {
    bool value;
    uint16_t counter;
    uint16_t timer;
    uint16_t count;
    uint16_t interval;
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
            .counter = 0,
            .timer = 0,
            .count = 25,
            .interval = 15,
        },
        .memory = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 25,
            .interval = 15,
        },
        .I2C = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
        },
        .RX8900SA = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
        },
        .U2 = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
        },
        .U3 = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
        },
        .U4 = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
        },
        .watchdog = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
        },
        .debug = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
        },
        .unknown = {
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
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

    case ERR_UNKNOWN:
        return &STATE.errors.unknown;
    }

    return NULL;
}

void syserr_tick() {
    err errors[] = {
        ERR_I2C_GENERIC,
        ERR_I2C_TIMEOUT,
        ERR_QUEUE_FULL,
        ERR_MEMORY,
        ERR_RX8900SA,
        ERR_U2,
        ERR_U3,
        ERR_U4,
        ERR_WATCHDOG,
        ERR_DEBUG,
        ERR_UNKNOWN,
    };

    int N = sizeof(errors) / sizeof(err);

    for (int i = 0; i < N; i++) {
        _err *e = _find(errors[i]);
        if (e != NULL) {
            if (e->timer > 0) {
                e->timer--;
            }
        }
    }
}

void set_error(err error, const char *tag, const char *fmt, ...) {
    _err *e = _find(error);

    if (e != NULL) {
        e->value = true;

        bool loggable = (e->counter == 0) && (e->timer == 0);
        if (loggable) {
            e->counter = (e->counter + 1) % e->count;
            e->timer = e->interval;

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
