#include <tusb.h>

#include <breakout.h>
#include <log.h>
#include <sys.h>
#include <types/buffer.h>
#include <usb.h>

#define LOGTAG "USB"

const uint8_t CDC1 = 1;

struct {
    repeating_timer_t usb_timer;

    struct {
        circular_buffer cli;
        circular_buffer ssmp;
    } buffers;

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

    .connected = {
        .usb0 = false,
        .usb1 = false,
    },

};

bool on_usb_rx(repeating_timer_t *rt);

bool usb_init() {
    USB.connected.usb0 = false;
    USB.connected.usb1 = false;

    mutex_init(&USB.guard.usb0);
    mutex_init(&USB.guard.usb1);

    add_repeating_timer_ms(50, on_usb_rx, NULL, &USB.usb_timer);

    infof(LOGTAG, "initialised");

    return true;
}

void usb_write(const uint8_t *bytes, int len) {
    mutex_enter_blocking(&USB.guard.usb1);
    if (USB.connected.usb1) {

        int ix = 0;
        while (ix < len) {
            uint32_t N = tud_cdc_n_write(CDC1, &bytes[ix], len - ix);

            if (N == 0) {
                warnf(LOGTAG, "*** write error %u of %d", ix, len);
                break;
            } else {
                debugf(LOGTAG, "write %u of %d", N, len - ix);
                ix += N;
            }
        }

        tud_cdc_n_write_flush(CDC1);

    } else {
        warnf(LOGTAG, "write error: USB not connected");
    }
    mutex_exit(&USB.guard.usb1);
}

bool on_usb_rx(repeating_timer_t *rt) {
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

    tud_task();

    return true;
}

// tinusb callback for received data
void tud_cdc_rx_cb(uint8_t itf) {
    uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

    // | IMPORTANT: also do this for CDC0 because otherwise
    // | you won't be able to print anymore to CDC0
    // | next time this function is called
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
