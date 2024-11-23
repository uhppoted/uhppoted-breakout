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
            circular_buffer *b = &USB.buffer;
            uint32_t msg = MSG_TTY | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000

            if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                set_error(ERR_QUEUE_FULL, "USB", "rx: queue full");
            }
        }
    }

    tud_task();

    return true;
}
