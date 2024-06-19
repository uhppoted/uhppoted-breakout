#pragma once

typedef struct IIR {
    float x;
    float y;
    float a[2];
    float b[2];
} IIR;

extern float IIR_process(IIR *iir, float in);