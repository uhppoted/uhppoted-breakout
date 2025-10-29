#pragma once

#include <stdbool.h>

extern bool usb_init();
extern bool usb1_write(const uint8_t *bytes, int N);
