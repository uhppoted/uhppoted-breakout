#include <dio.h>
#include <log.h>

void inputs(uint8_t v) {
    debugf("io", "inputs %02x %08b", v, v);
}
