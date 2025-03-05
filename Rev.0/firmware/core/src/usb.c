// #include <stdlib.h>

#include <pico/time.h>
#include <tusb.h>

// #include <breakout.h>
#include <log.h>
#include <usb.h>
// #include <types/buffer.h>

#define LOGTAG "USB"

struct {
    repeating_timer_t usb_timer;
    //     circular_buffer buffer;
    struct {
        bool usb0;
        bool usb1;
    } connected;
} USB = {
    //     .buffer = {
    //         .head = 0,
    //         .tail = 0,
    //     },

    .connected = {
        .usb0 = false,
        .usb1 = false,
    },
};

bool on_usb_rx(repeating_timer_t *rt);

bool usb_init() {
    USB.connected.usb0 = false;
    USB.connected.usb1 = false;

    add_repeating_timer_ms(50, on_usb_rx, NULL, &USB.usb_timer);

    infof(LOGTAG, "initialised");

    return true;
}

bool on_usb_rx(repeating_timer_t *rt) {
    if (tud_cdc_n_connected(0) && !USB.connected.usb0) {
        USB.connected.usb0 = true;

        infof(LOGTAG, "USB.0 connected");
    } else if (!tud_cdc_n_connected(0) && USB.connected.usb0) {
        USB.connected.usb0 = false;

        infof(LOGTAG, "USB.0 disconnected");
    }

    if (tud_cdc_n_connected(1) && !USB.connected.usb1) {
        USB.connected.usb1 = true;

        infof(LOGTAG, "USB.1 connected");
    } else if (!tud_cdc_n_connected(1) && USB.connected.usb1) {
        USB.connected.usb1 = false;

        infof(LOGTAG, "USB.1 disconnected");
    }

    //     if (tud_cdc_connected()) {
    //         int count = 0;
    //
    //         while (tud_cdc_available()) {
    //             uint8_t ch;
    //             uint32_t N = tud_cdc_read(&ch, 1);
    //
    //             if (N > 0) {
    //                 count++;
    //                 buffer_push(&USB.buffer, ch);
    //             }
    //         }
    //
    //         if (count > 0) {
    //             infof(LOGTAG, "AWOOOGAH");
    //             message qmsg = {
    //                 .message = MSG_RX,
    //                 .tag = MESSAGE_BUFFER,
    //                 .buffer = &USB.buffer,
    //             };
    //
    //             push(qmsg);
    //         }
    //     }

    tud_task();

    return true;
}

// Callback function for received data
void tud_cdc_rx_cb(uint8_t itf) {
    uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

    // | IMPORTANT: also do this for CDC0 because otherwise
    // | you won't be able to print anymore to CDC0
    // | next time this function is called
    uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

    // USB.1 ?
    if (itf == 1) {
        buf[count] = 0; // null-terminate the string
        // now echo data back to the console on CDC 0
        debugf(LOGTAG, "rx: %s", buf);

        // and echo back OK on CDC 1
        tud_cdc_n_write(itf, (uint8_t const *)"ok\r\n", 4);
        tud_cdc_n_write_flush(itf);
    }
}

// bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
//     debugf(LOGTAG, "tud_vendor_control_xfer_cb %02x %02x", request->bmRequestType, request->bRequest);
//
//     if (stage != CONTROL_STAGE_SETUP) {
//         return true;
//     }
//
//     // // BOOTSEL reboot command (0x92) ?
//     // if ((request->bmRequestType == 0x40) && (request->bRequest == 0x92)) {
//     //     watchdog_reboot(0, 0, 0);
//     //     *((uint32_t *)0x20041FFC) = 0x64738219;  // Magic value for BOOTSEL mode
//     //     scb_hw->aircr = (0x5FA << 16) | (1 << 2);
//     //     while (1);
//     // }
//
//     return false;
// }
