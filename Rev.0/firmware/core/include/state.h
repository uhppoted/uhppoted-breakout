#pragma once

#include <breakout.h>

void set_error(err e, const char *tag, const char *fmt, ...);
bool get_error(err e);
uint16_t get_errors();
