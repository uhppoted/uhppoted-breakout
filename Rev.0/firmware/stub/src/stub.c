#include <log.h>
#include <stub.h>

#define LOGTAG "SYS"

extern queue_t queue;

struct {
    struct {
        bool queue;
        bool memory;
        bool I2C;
        bool RX8900SA;
        bool U2;
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
        .U2 = false,
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
    }

    if (queue_is_full(&queue) || !queue_try_add(&queue, &m)) {
        warnf(LOGTAG, "queue full");
        return false;
    }

    return true;
}
