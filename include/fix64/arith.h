#pragma once

#include "fix64.h"
#include "fix64/impl.h"

//==========================================================
// Arithmetic functions
//==========================================================

/// Negates a fix64_t number. Note: fix64_neg(FIX64_MIN) returns
/// FIX64_MAX.
///
/// @param arg number to negate
/// @return the negative of arg
static inline fix64_t fix64_neg(fix64_t arg) {
    if (FIX64_UNLIKELY(arg.repr < -INT64_MAX)) {
        return FIX64_MAX;
    }
    return (fix64_t){ -arg.repr };
}

/// Addition of two fix64_t numbers
///
/// @param lhs left hand side for the addition
/// @param rhs right hand side for the addition
/// @return the sum of the two inputs
static inline fix64_t fix64_add(fix64_t lhs, fix64_t rhs) {
    return (fix64_t){ lhs.repr + rhs.repr };
}

/// Saturating addition of two fix64_t numbers
///
/// @param lhs left hand side for the addition
/// @param rhs right hand side for the addition
/// @return the sum of the two inputs
static inline fix64_t fix64_add_sat(fix64_t lhs, fix64_t rhs) {
    fix64_t result;
    int overflow = fix64_impl_add_i64_overflow(lhs.repr, rhs.repr, &result.repr);
    if (FIX64_UNLIKELY(overflow)) {
        result = (rhs.repr < 0) ? FIX64_MIN : FIX64_MAX;
    }
    return result;
}

/// Subtraction of two fix64_t numbers
///
/// @param lhs left hand side for the subtraction
/// @param rhs right hand side for the subtraction
/// @return the difference of the two inputs
static inline fix64_t fix64_sub(fix64_t lhs, fix64_t rhs) {
    return (fix64_t){ lhs.repr - rhs.repr };
}

/// Saturating subtraction of two fix64 numbers
///
/// @param lhs left hand side for the subtraction
/// @param rhs right hand side for the subtraction
/// @return the difference of the two inputs
static inline fix64_t fix64_sub_sat(fix64_t lhs, fix64_t rhs) {
    fix64_t result;
    int overflow = fix64_impl_sub_i64_underflow(lhs.repr, rhs.repr, &result.repr);
    if (FIX64_UNLIKELY(overflow)) {
        result = (rhs.repr > 0) ? FIX64_MIN : FIX64_MAX;
    }
    return result;
}

/// Multiplication of two fix64_t numbers
///
/// @param lhs left hand side for the multiplication
/// @param rhs right hand side for the multiplication
/// @return the product of the two inputs
static inline fix64_t fix64_mul(fix64_t lhs, fix64_t rhs) {
    int64_t hi;
    uint64_t lo = fix64_impl_mul_i64_i128(lhs.repr, rhs.repr, &hi);
    lo = fix64_impl_add_i128(hi, lo, 0, (1ull << (FIX64_FRAC_BITS - 1)), &hi); // For rounding
    int64_t result = (hi << (64 - FIX64_FRAC_BITS)) | (lo >> FIX64_FRAC_BITS);
    return (fix64_t){ result };
}

/// Saturating multiplication of two fix64_t numbers
///
/// @param lhs left hand side for the multiplication
/// @param rhs right hand side for the multiplication
/// @return the product of the two inputs
static inline fix64_t fix64_mul_sat(fix64_t lhs, fix64_t rhs) {
    int64_t hi;
    uint64_t lo = fix64_impl_mul_i64_i128(lhs.repr, rhs.repr, &hi);
    lo = fix64_impl_add_i128(hi, lo, 0, (1ull << (FIX64_FRAC_BITS - 1)), &hi); // For rounding
    int64_t result = (hi << (64 - FIX64_FRAC_BITS)) | (lo >> FIX64_FRAC_BITS);
    if (FIX64_UNLIKELY(hi > (FIX64_MAX.repr >> FIX64_FRAC_BITS))) {
        result = FIX64_MAX.repr;
    } else if (FIX64_UNLIKELY(hi < (FIX64_MIN.repr >> FIX64_FRAC_BITS))) {
        result = FIX64_MIN.repr;
    }
    return (fix64_t){ result };
}

/// Division of two fix64_t numbers. Result is rounded to the nearest representable value
///
/// @param lhs dividend side for the division
/// @param rhs divisor side for the division
/// @return the quotient of the two inputs
static inline fix64_t fix64_div(fix64_t lhs, fix64_t rhs) {
    int64_t hi = lhs.repr >> (64 - FIX64_FRAC_BITS);
    uint64_t lo = (uint64_t)lhs.repr << FIX64_FRAC_BITS;

    uint64_t round_lo = rhs.repr / 2;
    int64_t round_hi = rhs.repr >> 63; // sign extend
    if ((rhs.repr < 0) == (lhs.repr < 0)) {
        // if signs are same -> result is positive -> add for rounding
        lo = fix64_impl_add_i128(hi, lo, round_hi, round_lo, &hi);
    } else {
        // else subtract for rounding
        lo = fix64_impl_sub_i128(hi, lo, round_hi, round_lo, &hi);
    }

    int64_t rem;
    int64_t result = fix64_impl_div_i128_i64(hi, lo, rhs.repr, &rem);

    return (fix64_t){ (int64_t)result };
}

/// Saturating division of two fix64_t numbers. Result is rounded to the nearest representable
/// value. Division by zero is saturated depending on the sign of the dividend
///
/// @param lhs dividend side for the division
/// @param rhs divisor side for the division
/// @return the quotient of the two inputs
static inline fix64_t fix64_div_sat(fix64_t lhs, fix64_t rhs) {
    int64_t hi = lhs.repr >> (64 - FIX64_FRAC_BITS);
    uint64_t lo = (uint64_t)lhs.repr << FIX64_FRAC_BITS;

    if ((rhs.repr < 0) == (lhs.repr < 0)) {
        // if signs are same -> result is positive -> add for rounding
        lo = fix64_impl_add_i128(hi, lo, 0, rhs.repr / 2, &hi);
    } else {
        // else subtract for rounding
        lo = fix64_impl_sub_i128(hi, lo, 0, rhs.repr / 2, &hi);
    }

    int64_t rem;
    int64_t result = fix64_impl_div_i128_i64_sat(hi, lo, rhs.repr, &rem);

    return (fix64_t){ (int64_t)result };
}
