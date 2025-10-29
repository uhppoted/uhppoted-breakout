#include <tusb.h>

#include <breakout.h>
#include <log.h>
#include <sys.h>
#include <types/buffer.h>
#include <usb.h>

#define LOGTAG "USB"
#define TX_BUFFER_SIZE 1024

const uint8_t CDC1 = 1;

struct {
    repeating_timer_t usb_timer;

    struct {
        circular_buffer cli;
        circular_buffer ssmp;
    } buffers;

    struct {
        struct {
            volatile uint32_t head;
            volatile uint32_t tail;
            volatile uint32_t free;
            uint8_t buffer[TX_BUFFER_SIZE];
        } ssmp;
    } fifo;

    struct {
        bool usb0;
        bool usb1;
    } connected;

    struct {
        mutex_t usb0;
        mutex_t usb1;
    } guard;
} USB = {
    .buffers = {
        .cli = {
            .head = 0,
            .tail = 0,
        },

        .ssmp = {
            .head = 0,
            .tail = 0,
        },
    },

    .fifo = {
        .ssmp = {
            .head = 0,
            .tail = 0,
            .free = TX_BUFFER_SIZE,
        },
    },

    .connected = {
        .usb0 = false,
        .usb1 = false,
    },
};

bool usb_tick(repeating_timer_t *rt);

bool usb_init() {
    USB.connected.usb0 = false;
    USB.connected.usb1 = false;

    mutex_init(&USB.guard.usb0);
    mutex_init(&USB.guard.usb1);

    // NTS: should be in the region of 2-10ms:
    //      - 50ms works but is sluggish
    //      - 10ms allows for 5 retries with some leeway
    //      - TBH haven't ever seen a retry
    add_repeating_timer_ms(10, usb_tick, NULL, &USB.usb_timer);

    infof(LOGTAG, "initialised");

    return true;
}

bool usb_tick(repeating_timer_t *rt) {
    if (tud_cdc_n_connected(0) && !USB.connected.usb0) {
        USB.connected.usb0 = true;

        infof(LOGTAG, "USB.0 connected");
        stdout_connected(true);
    } else if (!tud_cdc_n_connected(0) && USB.connected.usb0) {
        USB.connected.usb0 = false;

        infof(LOGTAG, "USB.0 disconnected");
        stdout_connected(false);
    }

    if (tud_cdc_n_connected(1) && !USB.connected.usb1) {
        USB.connected.usb1 = true;

        infof(LOGTAG, "USB.1 connected");
    } else if (!tud_cdc_n_connected(1) && USB.connected.usb1) {
        USB.connected.usb1 = false;

        infof(LOGTAG, "USB.1 disconnected");
    }

    if (tud_cdc_n_connected(1) && USB.fifo.ssmp.tail != USB.fifo.ssmp.head) {
        int head = USB.fifo.ssmp.head;
        int tail = USB.fifo.ssmp.tail;
        int pending = (head + TX_BUFFER_SIZE - tail) % TX_BUFFER_SIZE;
        int len = pending < 64 ? pending : 64;
        uint8_t buffer[64];

        for (int i = 0; i < len; i++) {
            buffer[i] = USB.fifo.ssmp.buffer[tail];
            tail = (tail + 1) % TX_BUFFER_SIZE;
        }

        mutex_enter_blocking(&USB.guard.usb1);
        USB.fifo.ssmp.tail = tail;
        USB.fifo.ssmp.free += len;
        mutex_exit(&USB.guard.usb1);

        int retries = 0;

        while (true) {
            uint32_t N = tud_cdc_n_write(CDC1, buffer, len);

            if (N == 0 && retries > 5) {
                warnf(LOGTAG, "*** write error %u of %d", 0, len);
                break;
            } else if (N == 0) {
                warnf(LOGTAG, "*** write error - retrying");
                retries++;
                sleep_ms(1);
                tud_task();
            } else {
                tud_cdc_n_write_flush(CDC1);

                debugf(LOGTAG, "write %u of %d", N, len);
                break;
            }
        }
    }

    tud_task();

    return true;
}

// USB.1 only - use stdout for USB.0
bool usb1_write(const uint8_t *bytes, int len) {
    // ... connected ?
    if (!USB.connected.usb1) {
        warnf(LOGTAG, "write error: USB not connected");
        return false;
    }

    // ... sufficient space in FIFO ?
    if (USB.fifo.ssmp.free < len) {
        warnf(LOGTAG, "write error: insufficient buffer space");
        return false;
    }

    // ... copy to TX buffer
    uint32_t head = USB.fifo.ssmp.head;
    uint32_t tail = USB.fifo.ssmp.tail;
    for (int i = 0; i < len; i++) {
        USB.fifo.ssmp.buffer[head] = bytes[i];

        head = (head + 1) % TX_BUFFER_SIZE;
    }

    mutex_enter_blocking(&USB.guard.usb1);
    USB.fifo.ssmp.head = head;
    USB.fifo.ssmp.free -= len;
    mutex_exit(&USB.guard.usb1);

    debugf(LOGTAG, "queued  head:%d  tail:%d  len:%d", head, tail, len);

    return true;
}

// tinyusb callback for received data
void tud_cdc_rx_cb(uint8_t itf) {
    uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

    // NB: also do this for CDC0 because otherwise you won't be able to
    //     print anymore to CDC0 next time this function is called
    uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

    // USB.0 ?
    if (itf == 0 && count > 0) {
        for (int i = 0; i < count; i++) {
            buffer_push(&USB.buffers.cli, buf[i]);
        }

        message qmsg = {
            .message = MSG_TTY,
            .tag = MESSAGE_BUFFER,
            .buffer = &USB.buffers.cli,
        };

        push(qmsg);
    }

    // USB.1 ?
    if (itf == 1 && count > 0) {
        for (int i = 0; i < count; i++) {
            buffer_push(&USB.buffers.ssmp, buf[i]);
        }

        message qmsg = {
            .message = MSG_RX,
            .tag = MESSAGE_BUFFER,
            .buffer = &USB.buffers.ssmp,
        };

        push(qmsg);
    }
}
