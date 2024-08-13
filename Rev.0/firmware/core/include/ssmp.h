#pragma once

struct buffer;

extern void ssmp_init();
extern void ssmp_start();
extern void ssmp_ping();
extern void ssmp_rx(const struct buffer *);
