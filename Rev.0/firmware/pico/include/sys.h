#pragma once

#include <stdint.h>

extern const uint UART0_TX;
extern const uint UART0_RX;

extern void dispatch(uint32_t);
extern void setup_uart();
extern void blink();
