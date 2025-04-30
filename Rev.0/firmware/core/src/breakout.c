#include <stdarg.h>
#include <stdio.h>

#include <breakout.h>
#include <log.h>

#define LOGTAG "SYS"

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

typedef struct _err {
    err id;
    bool value;
    uint16_t counter;
    uint16_t timer;
    uint16_t count;
    uint16_t interval;
    uint16_t mask;
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
            .id = ERR_QUEUE_FULL,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 25,
            .interval = 15,
            .mask = BITMASK_ERR_QUEUE,
        },
        .memory = {
            .id = ERR_MEMORY,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 25,
            .interval = 15,
            .mask = BITMASK_ERR_MEMORY,
        },
        .I2C = {
            .id = ERR_I2C,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_I2C,
        },
        .RX8900SA = {
            .id = ERR_RX8900SA,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_RX8900SA,
        },
        .U2 = {
            .id = ERR_U2,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_U2,
        },
        .U3 = {
            .id = ERR_U3,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_U3,
        },
        .U4 = {
            .id = ERR_U4,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_U4,
        },
        .watchdog = {
            .id = ERR_WATCHDOG,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_WATCHDOG,
        },
        .debug = {
            .id = ERR_DEBUG,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_DEBUG,
        },
        .unknown = {
            .id = ERR_UNKNOWN,
            .value = false,
            .counter = 0,
            .timer = 0,
            .count = 1,
            .interval = 0,
            .mask = BITMASK_ERR_UNKNOWN,
        },
    },
};

extern queue_t queue;

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
        syserr_set(ERR_QUEUE_FULL, LOGTAG, "queue full");
        return false;
    }

    return true;
}

_err *_find(err error) {
    _err *errors[] = {
        &STATE.errors.I2C,
        &STATE.errors.queue,
        &STATE.errors.memory,
        &STATE.errors.RX8900SA,
        &STATE.errors.U2,
        &STATE.errors.U3,
        &STATE.errors.U4,
        &STATE.errors.watchdog,
        &STATE.errors.debug,
        &STATE.errors.unknown,
    };

    int N = sizeof(errors) / sizeof(_err *);

    for (int i = 0; i < N; i++) {
        _err *e = errors[i];

        if (e->id == error) {
            return e;
        }
    }

    return NULL;
}

void syserr_tick() {
    err errors[] = {
        ERR_I2C,
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

void syserr_set(err error, const char *tag, const char *fmt, ...) {
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

bool syserr_get(err error) {
    _err *e = _find(error);

    if (e != NULL) {
        return e->value;
    }

    return false;
}

uint16_t syserr_bitmask() {
    _err *errors[] = {
        &STATE.errors.I2C,
        &STATE.errors.queue,
        &STATE.errors.memory,
        &STATE.errors.RX8900SA,
        &STATE.errors.U2,
        &STATE.errors.U3,
        &STATE.errors.U4,
        &STATE.errors.watchdog,
        &STATE.errors.debug,
        &STATE.errors.unknown,
    };

    int N = sizeof(errors) / sizeof(_err *);

    uint16_t bits = 0x0000;
    for (int i = 0; i < N; i++) {
        _err *e = errors[i];

        bits |= e->value ? e->mask : 0x0000;
    }

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
