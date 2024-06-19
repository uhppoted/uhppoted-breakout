#include <IIR.h>

float IIR_process(IIR *iir, float in) {
    float out = 0.0;

    out = iir->b[0] * in;
    out += iir->b[1] * iir->x;
    out -= iir->a[1] * iir->y;

    iir->x = in;
    iir->y = out;

    return out;
}
