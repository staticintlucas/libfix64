#include "fix64.h"

#include <stddef.h>
#include <stdint.h>

#include "math/trig.inc"

fix64_t fix64_sin(fix64_t angle) {
    // Normalise so that 1.0 = pi/4 = 45deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, CHEBYSHEV_4_PI, &angle_hi); // Q32.94

    // Modulo 8 (2pi, i.e. 360deg)
    unsigned hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ull << (hi_frac_bits + 3)) - 1; // UQ3.94

    // Normalise to Q3.62, so angle is in the range [-pi/4, pi/4)
    angle_lo =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // UQ3.62
    // Constrain to range -1..1 (i.e. -pi/4..pi/4)
    angle_lo =
        ((angle_lo + CHEBYSHEV_ONE) & (CHEBYSHEV_TWO - 1)) - CHEBYSHEV_ONE; // Q2.62 as unsigned
    // Avoid UB of unsigned -> signed conversion overflow
    int64_t norm_a =
        (angle_lo > INT64_MAX) ? (int64_t)(angle_lo - INT64_MIN) + INT64_MIN : (int64_t)angle_lo;
    // Which eighth of the unit circle the angle lies in
    unsigned octant = (angle_hi >> hi_frac_bits) + 1; // 1-8

    int neg_result = (octant & 4) != 0; // negate result for 4,5,6,7
    int use_cos = (octant & 2) != 0; // use cos for 2,3,6,7

    int64_t result = 0;
    if (use_cos) {
        result = chebyshev_cos_impl(norm_a); // Q0.63
    } else {
        result = chebyshev_sin_impl(norm_a); // Q0.63
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

    // Normalise to Q3.62, so angle is in the range [-pi/4, pi/4)
    angle_lo =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // UQ3.62
    // Constrain to range -1..1 (i.e. -pi/4..pi/4)
    angle_lo =
        ((angle_lo + CHEBYSHEV_ONE) & (CHEBYSHEV_TWO - 1)) - CHEBYSHEV_ONE; // Q2.62 as unsigned
    // Avoid UB of unsigned -> signed conversion overflow
    int64_t norm_a =
        (angle_lo > INT64_MAX) ? (int64_t)(angle_lo - INT64_MIN) + INT64_MIN : (int64_t)angle_lo;
    // Which eighth of the unit circle the angle lies in
    unsigned octant = (angle_hi >> hi_frac_bits) + 1; // 1-8

    int neg_result = ((octant >> 1) ^ (octant >> 2)) & 1; // negate result for 2,3,4,5
    int use_sin = (octant & 2) != 0; // use cos for 2,3,6,7

    int64_t result = 0;
    if (use_sin) {
        result = chebyshev_sin_impl(norm_a);
    } else {
        result = chebyshev_cos_impl(norm_a);
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

    // Modulo 8 (2pi, i.e. 360deg)
    unsigned hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ull << (hi_frac_bits + 3)) - 1; // UQ3.94

    // Normalise to Q3.62, so angle is in the range [-pi/4, pi/4)
    angle_lo =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // UQ3.62
    // Constrain to range -1..1 (i.e. -pi/4..pi/4)
    angle_lo =
        ((angle_lo + CHEBYSHEV_ONE) & (CHEBYSHEV_TWO - 1)) - CHEBYSHEV_ONE; // Q2.62 as unsigned
    // Avoid UB of unsigned -> signed conversion overflow
    int64_t norm_a =
        (angle_lo > INT64_MAX) ? (int64_t)(angle_lo - INT64_MIN) + INT64_MIN : (int64_t)angle_lo;
    // Which eighth of the unit circle the angle lies in
    unsigned octant = (angle_hi >> hi_frac_bits) + 1; // 1-8

    int recip_result = (octant & 2) != 0; // 1/result for 2,3,6,7

    int64_t result = 0;
    if (recip_result) {
        result = chebyshev_tan_impl(-norm_a);
        // tan(x) = 1/tan(pi/2-x)
        result = fix64_impl_div_i128_i64_sat(
            1ull << (CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64), 0, result);
    } else {
        result = chebyshev_tan_impl(norm_a); // Q1.62
        // Round to Q1.32
        result += (1ull << (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS - 1));
        result >>= (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32
    }

    return (fix64_t){ result };
}
