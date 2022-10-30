#include "fix64.h"

#include <stddef.h>
#include <stdint.h>

#include "math/trig.inc"

fix64_t fix64_sin(fix64_t angle) {
    // Normalise so that 1.0 = pi/2 = 90deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, CHEBYSHEV_2_PI, &angle_hi); // Q31.94

    // Calculate octant, used to determine which chebyshev function to call
    uint32_t hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    unsigned octant = (angle_hi >> (hi_frac_bits - 1)) & 0x7;
    // Modulo pi/2 (90deg)
    angle_hi &= (1ull << hi_frac_bits) - 1; // UQ0.94

    // Normalise -pi/4..pi/4 (90deg) to range -1..1 with CHEBYSHEV_FRAC_BITS fractional bits
    int64_t norm_a =
        (angle_hi << (64 - (FIX64_FRAC_BITS - 1))) | (angle_lo >> (FIX64_FRAC_BITS - 1)); // Q1.62
    norm_a =
        (((norm_a + (1ull << CHEBYSHEV_FRAC_BITS)) & ((2ull << CHEBYSHEV_FRAC_BITS) - 1)) -
         (1ull << CHEBYSHEV_FRAC_BITS)); // Q1.62

    int64_t result = 0;
    switch (octant) {
        case 7:
        case 0:
            result = chebyshev_sin_impl(norm_a);
            break;
        case 1:
        case 2:
            result = chebyshev_cos_impl(norm_a);
            break;
        case 3:
        case 4:
            result = -chebyshev_sin_impl(norm_a);
            break;
        case 5:
        case 6:
            result = -chebyshev_cos_impl(norm_a);
            break;
    }

    // Round to Q31.32
    result += (INT64_C(1) << (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS - 1));
    result >>= (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32

    return (fix64_t){ result };
}

fix64_t fix64_cos(fix64_t angle) {
    // Normalise so that 1.0 = pi/2 = 90deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, CHEBYSHEV_2_PI, &angle_hi); // Q31.94

    // Calculate octant, used to determine which chebyshev function to call
    uint32_t hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    unsigned octant = (angle_hi >> (hi_frac_bits - 1)) & 0x7;
    // Modulo pi/2 (90deg)
    angle_hi &= (1ull << hi_frac_bits) - 1; // UQ0.94

    // Normalise -pi/4..pi/4 (90deg) to range -1..1 with CHEBYSHEV_FRAC_BITS fractional bits
    int64_t norm_a =
        (angle_hi << (64 - (FIX64_FRAC_BITS - 1))) | (angle_lo >> (FIX64_FRAC_BITS - 1)); // Q1.62
    norm_a =
        (((norm_a + (1ull << CHEBYSHEV_FRAC_BITS)) & ((2ull << CHEBYSHEV_FRAC_BITS) - 1)) -
         (1ull << CHEBYSHEV_FRAC_BITS)); // Q1.62

    int64_t result = 0;
    switch (octant) {
        case 7:
        case 0:
            result = chebyshev_cos_impl(norm_a);
            break;
        case 1:
        case 2:
            result = -chebyshev_sin_impl(norm_a);
            break;
        case 3:
        case 4:
            result = -chebyshev_cos_impl(norm_a);
            break;
        case 5:
        case 6:
            result = chebyshev_sin_impl(norm_a);
            break;
    }

    // Round to Q31.32
    result += (INT64_C(1) << (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS - 1));
    result >>= (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32

    return (fix64_t){ result };
}

fix64_t fix64_tan(fix64_t angle) {
    // Normalise so that 1.0 = pi/2 = 90deg
    int64_t angle_hi;
    uint64_t angle_lo = fix64_impl_mul_i64_i128(angle.repr, CHEBYSHEV_2_PI, &angle_hi); // Q31.94

    // Calculate octant, used to determine which chebyshev function to call
    uint32_t hi_frac_bits = CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64; // frac bits in angle_hi
    unsigned octant = (angle_hi >> (hi_frac_bits - 1)) & 0x7;
    // Modulo pi/2 (90deg)
    angle_hi &= (1ull << hi_frac_bits) - 1; // UQ0.94

    // Normalise -pi/4..pi/4 (90deg) to range -1..1 with CHEBYSHEV_FRAC_BITS fractional bits
    int64_t norm_a =
        (angle_hi << (64 - (FIX64_FRAC_BITS - 1))) | (angle_lo >> (FIX64_FRAC_BITS - 1)); // Q1.62
    norm_a =
        (((norm_a + (1ull << CHEBYSHEV_FRAC_BITS)) & ((2ull << CHEBYSHEV_FRAC_BITS) - 1)) -
         (1ull << CHEBYSHEV_FRAC_BITS)); // Q1.62

    int64_t result = 0;
    switch (octant) {
        case 7:
        case 0:
        case 3:
        case 4:
            result = chebyshev_tan_impl(norm_a); // Q1.62
            // Round to Q1.32
            result += (1ull << (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS - 1));
            result >>= (CHEBYSHEV_FRAC_BITS - FIX64_FRAC_BITS); // Q31.32
            break;
        case 1:
        case 2:
        case 5:
        case 6:
            result = chebyshev_tan_impl(-norm_a);
            if (!result) {
                return FIX64_MAX;
            }
            // tan(x) = 1/tan(pi/2-x)
            int64_t rem;
            result = fix64_impl_div_i128_i64(
                1ull << (CHEBYSHEV_FRAC_BITS + FIX64_FRAC_BITS - 64), 0, result, &rem);
            break;
    }

    return (fix64_t){ result };
}
