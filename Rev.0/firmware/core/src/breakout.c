#include <breakout.h>
#include <state.h>

// NTS: SRAM_BASE is 0x20000000
bool push(message msg) {
    uint32_t m = msg.message;

    switch (msg.tag) {
    case MESSAGE_UINT32:
        m |= msg.u32 & 0x0fffffff;
        break;

    case MESSAGE_BUFFER:
        m |= ((uint32_t)msg.buffer & 0x0fffffff);
        break;

    case MESSAGE_PIN:
        m |= ((uint32_t)msg.pin & 0x0fffffff);
        break;
    }

    if (queue_is_full(&queue) || !queue_try_add(&queue, &m)) {
        set_error(ERR_QUEUE_FULL, "SYS", "queue full");
        return false;
    }

    return true;
}
