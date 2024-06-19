#include <IIR.h>

// y₀ = (b₀x₀ + b₁x₁ - a₁y₁)/a₀
float IIR_process(IIR *iir, float x₀) {
    // clang-format off
    float y₀ = iir->b₀ * x₀;
          y₀ += iir->b₁ * iir->x₁;
          y₀ -= iir->a₁ * iir->y₁;
    // clang-format on

    iir->x₁ = x₀;
    iir->y₁ = y₀;

    return y₀;
}
