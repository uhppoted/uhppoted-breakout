#include <dio.h>
#include <log.h>

void inputs(uint8_t v, uint8_t mask) {
    debugf("io", "inputs %02x (%08b)  mask %02x (%08b)", v, v, mask, mask);
}
