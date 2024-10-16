#include <stdlib.h>

#include <pico/time.h>
#include <tusb.h>

#include <breakout.h>
#include <log.h>
#include <state.h>

repeating_timer_t usb_timer;

struct {
    bool connected;
} USB = {
    .connected = false,
};

bool on_usb_rx(repeating_timer_t *rt);

void usb_init() {
    USB.connected = false;
    add_repeating_timer_ms(10, on_usb_rx, NULL, &usb_timer);

    infof("USB", "initialised");
}

bool on_usb_rx(repeating_timer_t *rt) {
    if (tud_cdc_connected() && !USB.connected) {
        USB.connected = true;

        uint32_t msg = MSG_USB | 0x00000001;
        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "USB", "rx: queue full");
        }

    } else if (!tud_cdc_connected() && USB.connected) {
        USB.connected = false;

        uint32_t msg = MSG_USB | 0x00000000;
        if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
            set_error(ERR_QUEUE_FULL, "USB", "rx: queue full");
        }
    }

    if (tud_cdc_connected()) {
        uint8_t buffer[32];
        int ix = 0;

        while (tud_cdc_available() && ix < sizeof(buffer)) {
            uint8_t ch;
            uint32_t N = tud_cdc_read(&ch, 1);

            if (N > 0) {
                buffer[ix++] = ch;
            }
        }

        if (ix > 0) {
            struct buffer *b;

            if ((b = (struct buffer *)malloc(sizeof(struct buffer))) != NULL) {
                b->N = ix;
                memmove(b->data, buffer, ix);
                uint32_t msg = MSG_TTY | ((uint32_t)b & 0x0fffffff); // SRAM_BASE is 0x20000000
                if (queue_is_full(&queue) || !queue_try_add(&queue, &msg)) {
                    set_error(ERR_QUEUE_FULL, "USB", "rx: queue full");
                    free(b);
                }
            }
        }
    }

    tud_task();

    return true;
}
