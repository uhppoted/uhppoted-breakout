#include <log.h>
#include <readers.h>

void wio(uint8_t inputs) {
    debugf("reader", "... %02x %08b", inputs, inputs);
}
