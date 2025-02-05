#pragma once

#include <stdint.h>

#include <U2/swipe.h>

extern void U2_init();
extern void U2_start();
extern void U2_wio(uint8_t inputs);

extern void U2_free(swipe *swipe);
