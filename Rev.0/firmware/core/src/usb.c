#include <stdlib.h>

#include <pico/time.h>
#include <tusb.h>

#include <breakout.h>
#include <log.h>
#include <state.h>
#include <types/buffer.h>

repeating_timer_t usb_timer;

struct {
    circular_buffer buffer;
    bool connected;
} USB = {
    .buffer = {
        .head = 0,
        .tail = 0,
    },

    .connected = false,
};

bool on_usb_rx(repeating_timer_t *rt);

void usb_init() {
    USB.connected = false;
    add_repeating_timer_ms(50, on_usb_rx, NULL, &usb_timer);

    infof("USB", "initialised");
}

bool on_usb_rx(repeating_timer_t *rt) {
    if (tud_cdc_connected() && !USB.connected) {
        USB.connected = true;

        infof("USB", "connected");
    } else if (!tud_cdc_connected() && USB.connected) {
        USB.connected = false;

        infof("USB", "disconnected");
    }

    if (tud_cdc_connected()) {
        int count = 0;

        while (tud_cdc_available()) {
            uint8_t ch;
            uint32_t N = tud_cdc_read(&ch, 1);

            if (N > 0) {
                count++;
                buffer_push(&USB.buffer, ch);
            }
        }

        if (count > 0) {
            message qmsg = {
                .message = MSG_TTY,
                .tag = MESSAGE_BUFFER,
                .buffer = &USB.buffer,
            };

            push(qmsg);
        }
    }

    tud_task();

    return true;
}
