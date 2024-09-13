#pragma once

struct buffer;

extern void SSMP_init();
extern void SSMP_start();
extern void SSMP_reset();
extern void SSMP_ping();
extern void SSMP_rx(const struct buffer *);
