#include "fix64.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "math/trig.inc"

fix64_t fix64_sin(fix64_t angle) {
    // Normalise so that 1.0 = pi/4 = 45deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, TRIG_4_PI, &angle_hi); // Q31.94

    // Modulo 8 (2pi, i.e. 360deg)
    unsigned hi_frac_bits = TRIG_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ll << (hi_frac_bits + 3)) - 1; // Q3.94

    // Normalise to Q0.62 in the range [0, pi/4)
    int64_t norm_a =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // UQ2.62
    norm_a &= (TRIG_ONE - 1); // Q0.62

    // Which eighth of the unit circle the angle lies in determines what calculation is used
    // a = 0deg..45deg => sin(na)
    // a = 45deg..90deg => cos(45deg-na)
    // a = 90deg..135deg => cos(na)
    // a = 135deg..180deg => sin(45deg-na)
    // a = 180deg..225deg => -sin(na)
    // a = 225deg..270deg => -cos(45deg-na)
    // a = 270deg..315deg => -cos(na)
    // a = 315deg..360deg => -sin(45deg-na)
    unsigned octant = (angle_hi >> hi_frac_bits); // 0-7
    int neg_angle = (octant & 1) != 0; // flip input range for 1,3,5,7
    int neg_result = (octant & 4) != 0; // negate result for 4,5,6,7
    int use_cos = ((octant + 1) & 2) != 0; // use cos for 1,2,5,6

    int64_t result = 0;
    norm_a = neg_angle ? (TRIG_ONE - norm_a) : norm_a;
    if (use_cos) {
        result = chebyshev_cos_impl(norm_a); // Q0.62
    } else {
        result = chebyshev_sin_impl(norm_a); // Q0.62
    }
    result = neg_result ? -result : result;

    // Round to Q31.32
    result += (INT64_C(1) << (TRIG_FRAC_BITS - FIX64_FRAC_BITS - 1));
    result >>= (TRIG_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32

    return (fix64_t){ result };
}

fix64_t fix64_cos(fix64_t angle) {
    // Normalise so that 1.0 = pi/4 = 45deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, TRIG_4_PI, &angle_hi); // Q31.94

    // Modulo 8 (2pi, i.e. 360deg)
    unsigned hi_frac_bits = TRIG_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ll << (hi_frac_bits + 3)) - 1; // Q3.94

    // Normalise to Q0.62, so angle is in the range [0, pi/4)
    int64_t norm_a =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // Q1.62
    norm_a &= (TRIG_ONE - 1); // Q0.62

    // Which eighth of the unit circle the angle lies in determines what calculation is used
    // a = 0deg..45deg => cos(na)
    // a = 45deg..90deg => sin(45deg-na)
    // a = 90deg..135deg => -sin(na)
    // a = 135deg..180deg => -cos(45deg-na)
    // a = 180deg..225deg => -cos(na)
    // a = 225deg..270deg => -sin(45deg-na)
    // a = 270deg..315deg => sin(na)
    // a = 315deg..360deg => cos(45deg-na)
    unsigned octant = (angle_hi >> hi_frac_bits); // 0-7
    int neg_angle = (octant & 1) != 0; // flip input range for 1,3,5,7
    int neg_result = ((octant + 2) & 4) != 0; // negate result for 2,3,4,5
    int use_sin = ((octant + 1) & 2) != 0; // use sin for 1,2,5,6

    int64_t result = 0;
    norm_a = neg_angle ? (TRIG_ONE - norm_a) : norm_a;
    if (use_sin) {
        result = chebyshev_sin_impl(norm_a); // Q0.62
    } else {
        result = chebyshev_cos_impl(norm_a); // Q0.62
    }
    result = neg_result ? -result : result;

    // Round to Q31.32
    result += (INT64_C(1) << (TRIG_FRAC_BITS - FIX64_FRAC_BITS - 1));
    result >>= (TRIG_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32

    return (fix64_t){ result };
}

fix64_t fix64_tan(fix64_t angle) {
    // Normalise so that 1.0 = pi/8 = 22.5deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_u64_i128(angle.repr, TRIG_8_PI, &angle_hi); // Q33.94

    // Modulo 8 (pi, i.e. 180deg) since tan repeats after that
    unsigned hi_frac_bits = TRIG_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    angle_hi &= (1ll << (hi_frac_bits + 3)) - 1; // Q3.94

    // Normalise to Q0.62, so angle is in the range [0, pi/8)
    int64_t norm_a =
        ((uint64_t)angle_hi << (64 - FIX64_FRAC_BITS)) | (angle_lo >> FIX64_FRAC_BITS); // Q1.62
    norm_a &= (TRIG_ONE - 1); // Q0.62

    // Which 16th of the unit circle the angle lies in determines what calculation is used
    // a = 0deg..22.5deg => tan(na)
    // a = 22.5deg..45deg => (1 - tan(45deg-na)) / (1 + tan(45deg-na))
    // a = 45deg..67.5deg => (1 + tan(na)) / (1 - tan(na))
    // a = 67.5deg..90deg => 1 / tan(45deg-na)
    // a = 90deg..112.5deg => -1 / tan(na)
    // a = 112.5deg..135deg => -(1 + tan(45deg-na)) / (1 - tan(45deg-na))
    // a = 135deg..157.5deg => -(1 - tan(na)) / (1 + tan(na))
    // a = 157.5deg..180deg => -tan(45deg-na)
    unsigned hexadecant = (angle_hi >> hi_frac_bits); // 0-7
    int neg_angle = (hexadecant & 1) != 0; // flip input range for 1,3,5,7
    int neg_result = (hexadecant & 4) != 0; // negate result for 4,5,6,7
    int recip_result = ((hexadecant + 2) & 4) != 0; // 1/result for 2,3,4,5
    int angle_sum = ((hexadecant + 1) & 2) != 0; // Use angle sum formula for 1,2,5,6

    int64_t result = 0;
    norm_a = neg_angle ? (TRIG_ONE - norm_a) : norm_a;
    result = chebyshev_tan_impl(norm_a); // Q1.62

    int64_t num, denom;
    if (angle_sum) {
        num = TRIG_ONE - result; // Q1.62
        denom = TRIG_ONE + result; // Q1.62
    } else {
        num = result; // Q1.62
        denom = TRIG_ONE; // Q1.62
    }

    if (recip_result) {
        int64_t tmp = denom;
        denom = num;
        num = tmp;
    }

    int64_t num_hi = num >> (64 - FIX64_FRAC_BITS);
    uint64_t num_lo = num << FIX64_FRAC_BITS; // Q1.94
    // fix(a/b + rounding) = fix((a+b*rounding)/b)
    num_lo = fix64_impl_add_i128(num_hi, num_lo, 0, denom >> 1, &num_hi);
    denom = neg_result ? -denom : denom;
    result = fix64_impl_div_i128_i64(num_hi, num_lo, denom);

    return (fix64_t){ result };
}
