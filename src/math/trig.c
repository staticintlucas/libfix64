#include "fix64.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "math/trig.inc"

fix64_t fix64_sin(fix64_t angle) {
    // Normalise so that 1.0 = pi/4 = 45deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, CHEBYSHEV_4_PI, &angle_hi); // Q31.94

    // Modulo 8 (2pi, i.e. 360deg)
    unsigned hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ull << (hi_frac_bits + 3)) - 1; // Q3.94

    // Normalise to Q0.62, so angle is in the range [0, pi/4)
    int64_t norm_a =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // Q1.62
    norm_a &= (CHEBYSHEV_ONE - 1); // Q0.62
    // Which eighth of the unit circle the angle lies in
    unsigned octant = (angle_hi >> hi_frac_bits); // 0-7

    int neg_angle = (octant & 1) != 0; // flip input range for 1,3,5,7
    int neg_result = (octant & 4) != 0; // negate result for 4,5,6,7
    int use_cos = ((octant + 1) & 2) != 0; // use cos for 1,2,5,6

    int64_t result = 0;
    norm_a = neg_angle ? (CHEBYSHEV_ONE - norm_a) : norm_a;
    if (use_cos) {
        result = chebyshev_cos_impl(norm_a); // Q0.62
    } else {
        result = chebyshev_sin_impl(norm_a); // Q0.62
    }
    result = neg_result ? -result : result;

    // Round to Q31.32
    result += (INT64_C(1) << (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS - 1));
    result >>= (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32

    return (fix64_t){ result };
}

fix64_t fix64_cos(fix64_t angle) {
    // Normalise so that 1.0 = pi/4 = 45deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, CHEBYSHEV_4_PI, &angle_hi); // Q31.94

    // Modulo 8 (2pi, i.e. 360deg)
    unsigned hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ull << (hi_frac_bits + 3)) - 1; // UQ3.94

    // Normalise to Q0.62, so angle is in the range [0, pi/4)
    int64_t norm_a =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // Q1.62
    norm_a &= (CHEBYSHEV_ONE - 1); // Q0.62
    // Which eighth of the unit circle the angle lies in
    unsigned octant = (angle_hi >> hi_frac_bits); // 0-7

    int neg_angle = (octant & 1) != 0; // flip input range for 1,3,5,7
    int neg_result = ((octant + 2) & 4) != 0; // negate result for 2,3,4,5
    int use_sin = ((octant + 1) & 2) != 0; // use sin for 1,2,5,6

    int64_t result = 0;
    norm_a = neg_angle ? (CHEBYSHEV_ONE - norm_a) : norm_a;
    if (use_sin) {
        result = chebyshev_sin_impl(norm_a); // Q0.62
    } else {
        result = chebyshev_cos_impl(norm_a); // Q0.62
    }
    result = neg_result ? -result : result;

    // Round to Q31.32
    result += (INT64_C(1) << (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS - 1));
    result >>= (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32

    return (fix64_t){ result };
}

fix64_t fix64_tan(fix64_t angle) {
    // Normalise so that 1.0 = pi/4 = 45deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, CHEBYSHEV_4_PI, &angle_hi); // Q31.94

    // Modulo 4 (pi, i.e. 180deg) since tan repeats after that
    unsigned hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ull << (hi_frac_bits + 2)) - 1; // Q2.94

    // Normalise to Q0.62, so angle is in the range [0, pi/4)
    int64_t norm_a =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // Q1.62
    norm_a &= (CHEBYSHEV_ONE - 1); // Q0.62
    // Which eighth of the unit circle the angle lies in
    unsigned octant = (angle_hi >> hi_frac_bits); // 0-3

    int neg_angle = (octant & 1) != 0; // flip input range for 1,3
    int neg_result = (octant & 2) != 0; // negate result for 2,3
    int recip_result = ((octant + 1) & 2) != 0; // 1/result for 1,2

    int64_t result = 0;
    norm_a = neg_angle ? (CHEBYSHEV_ONE - norm_a) : norm_a;
    if (recip_result) {
        result = chebyshev_tan_impl(norm_a); // Q1.62

        // tan(x) = 1/tan(pi/2-x) = 1/result
        // fix(1/x + rounding) = fix((1+x*rounding)/x)
        int64_t num_hi = (1ull << (CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64));
        uint64_t num_lo = result >> 1; // Q1.94

        result = neg_result ? -result : result;
        result = fix64_impl_div_i128_i64_sat(num_hi, num_lo, result); // Q31.32
    } else {
        result = chebyshev_tan_impl(norm_a); // Q1.62
        result = neg_result ? -result : result;
        // Round to Q1.32
        result += (1ull << (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS - 1));
        result >>= (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32
    }

    return (fix64_t){ result };
}
