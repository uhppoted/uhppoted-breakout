#pragma once

struct buffer;

extern void cli_rx(const struct buffer *);
extern void cli_init();
extern void cli_ping();