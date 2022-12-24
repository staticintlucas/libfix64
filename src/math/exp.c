#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "fix64.h"
#include "fix64/impl.h"

#include "math/exp.inc"

// Calculates 2**x-1 for UQ0.64 fixed point numbers
static uint64_t chebyshev_exp2m1_impl(uint64_t arg) {
    uint64_t sum = exp2m1_coefs[0]; // UQ0.64
    for (size_t i = 1; i < sizeof(exp2m1_coefs) / sizeof(exp2m1_coefs[0]); i++) {
        // Note: assumes EXP_FRAC_BITS == 64
        fix64_impl_mul_u64_u128(sum, arg, &sum); // UQ0.128 => take upper half for UQ0.64
        sum += exp2m1_coefs[i]; // UQ0.64
    }

    return sum; // UQ0.64
}

// Calculates log2(1+x) for UQ0.64 fixed point numbers
// Based on Clay Turner's paper: http://www.claysturner.com/dsp/BinaryLogarithm.pdf
static uint64_t turner_log21p_impl(uint64_t arg) {
    uint64_t result = 0;
    for (size_t i = 0; i < (FIX64_FRAC_BITS + 1); i++) {
        // (1+x)**2 - 1 == 2*x + x**2 == (x << 1) + (x * x)
        // Note: arg_shr64 assumes EXP_FRAC_BITS == 64
        uint64_t arg_shr64;
        fix64_impl_mul_u64_u128(arg, arg, &arg_shr64);
        uint64_t arg_hi;
        uint64_t arg_lo =
            fix64_impl_add_u128(arg >> 63, arg << 1, 0, arg_shr64, &arg_hi); // UQ64.64

        result <<= 1;
        if (arg_hi) { // i.e. if (arg_hi:arg_lo >= 1.0) // Note: assumes EXP_FRAC_BITS == 64
            // arg+1 = (arg+1)/2 => arg = (arg-1)/2
            // Note: assumes EXP_FRAC_BITS == 64
            arg = ((arg_hi - 1) << 63) | (arg_lo >> 1);
            result |= 1;
        } else {
            arg = arg_lo;
        }
    }
    return result << (EXP_FRAC_BITS - (FIX64_FRAC_BITS + 1));
}

static fix64_t fix64_exp2_inner(int64_t ipart, uint64_t fpart) {
    if (FIX64_UNLIKELY(ipart >= FIX64_INT_BITS)) {
        return FIX64_MAX;
    } else if (FIX64_UNLIKELY(ipart < -FIX64_FRAC_BITS - 1)) {
        return FIX64_ZERO;
    }

    // Note: assumes EXP_FRAC_BITS == 64
    uint64_t hi = 1;
    uint64_t lo = chebyshev_exp2m1_impl(fpart); // UQ1.64

    uint64_t round_hi = 0, round_lo = 0;
    uint32_t round_shift = EXP_FRAC_BITS - FIX64_FRAC_BITS - ipart;
    if ((round_shift - 1) >= 64) {
        round_hi = 1ull << ((round_shift - 1) - 64);
    } else {
        round_lo = 1ull << (round_shift - 1);
    }
    lo = fix64_impl_add_u128(hi, lo, round_hi, round_lo, &hi);

    if (round_shift >= 64) {
        return (fix64_t){ hi >> (round_shift - 64) };
    } else {
        return (fix64_t){ (hi << (64 - round_shift)) | (lo >> round_shift) };
    }
}

fix64_t fix64_exp(fix64_t arg) {
    int64_t arg_log2e_hi;
    uint64_t arg_log2e_lo =
        fix64_impl_mul_i64_u64_i128(arg.repr, exp_log2e_val, &arg_log2e_hi); // Q32.95

    uint32_t round_shift = FIX64_FRAC_BITS + MUL_FRAC_BITS - EXP_FRAC_BITS;
    arg_log2e_lo = fix64_impl_add_i128(
        arg_log2e_hi, arg_log2e_lo, 0, 1ull << (round_shift - 1), &arg_log2e_hi); // rounding
    arg_log2e_lo = (arg_log2e_hi << (64 - round_shift)) | (arg_log2e_lo >> round_shift);
    arg_log2e_hi >>= round_shift; // Q32.64

    // Note: assumes EXP_FRAC_BITS == 64
    return fix64_exp2_inner(arg_log2e_hi, arg_log2e_lo);
}

fix64_t fix64_exp2(fix64_t arg) {
    int64_t ipart = fix64_to_int(arg); // Q31.0

    uint64_t fmask = (UINT64_C(1) << FIX64_FRAC_BITS) - 1; // Mask for fractional bits
    uint64_t fpart = (arg.repr & fmask) << (EXP_FRAC_BITS - FIX64_FRAC_BITS); // UQ0.64

    return fix64_exp2_inner(ipart, fpart);
}

fix64_t fix64_log(fix64_t arg) {
    int64_t log2 = fix64_log2(arg).repr; // Q31.32

    // ln(x) = log2(x) / log2(e) = log2(x) * (1/log2(e))
    int64_t result_hi;
    uint64_t result_lo = fix64_impl_mul_i64_u64_i128(log2, log_1_log2e_val, &result_hi); // Q31.96
    result_lo = fix64_impl_add_i128(
        result_hi, result_lo, 0, 1ull << (EXP_FRAC_BITS - 1), &result_hi); // rounding

    // Note: assumes EXP_FRAC_BITS == 64
    return (fix64_t){ result_hi };
}

fix64_t fix64_log10(fix64_t arg) {
    int64_t log2 = fix64_log2(arg).repr; // Q31.32

    // log10(x) = log2(x) / log2(10) = log2(x) * (1 / log2(10))
    int64_t result_hi;
    uint64_t result_lo =
        fix64_impl_mul_i64_u64_i128(log2, log10_1_log2_10_val, &result_hi); // Q31.96
    result_lo = fix64_impl_add_i128(
        result_hi, result_lo, 0, 1ull << (EXP_FRAC_BITS - 1), &result_hi); // rounding

    // Note: assumes EXP_FRAC_BITS == 64
    return (fix64_t){ result_hi };
}

fix64_t fix64_log2(fix64_t arg) {
    if (FIX64_UNLIKELY(fix64_lte(arg, FIX64_ZERO))) {
        return FIX64_MIN;
    }

    int64_t lz = fix64_impl_clz64(arg.repr); // Q31.0
    uint64_t fpart = arg.repr << (lz + 1); // Q0.64

    // Note: assumes EXP_FRAC_BITS == 64
    int64_t result_hi = FIX64_INT_BITS - lz;
    uint64_t result_lo = turner_log21p_impl(fpart); // Q31.64

    uint32_t round_shift = EXP_FRAC_BITS - FIX64_FRAC_BITS;
    result_lo = fix64_impl_add_i128(
        result_hi, result_lo, 0, 1ull << (round_shift - 1), &result_hi); // rounding

    return (fix64_t){ (result_hi << (64 - round_shift)) | (result_lo >> round_shift) };
}
