#pragma once

typedef struct IIR {
    float x₁;
    float y₁;
    float a₀;
    float a₁;
    float b₀;
    float b₁;
} IIR;

extern float IIR_process(IIR *iir, float in);