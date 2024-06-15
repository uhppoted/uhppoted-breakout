#include <log.h>
#include <wiegand.h>

void wio(uint8_t inputs, uint8_t mask) {
    debugf("reader", "inputs:%02x (%08b)  mask:%02x (%08b)", inputs, inputs, mask, mask);
}
