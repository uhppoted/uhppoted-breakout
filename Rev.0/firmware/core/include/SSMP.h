#pragma once

struct circular_buffer;

extern void SSMP_init();
extern void SSMP_start();
extern void SSMP_reset();
extern void SSMP_rx(struct circular_buffer *);
