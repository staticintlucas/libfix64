#pragma once

#include "fix64.h"
#include "fix64/arith.h"

//==========================================================
// Basic math functions
//==========================================================

/// Rounds down to the nearest integral value less than or equal to the fixed point argument
///
/// @param arg the fixed point number to floor
/// @return the floor of arg
static inline fix64_t fix64_floor(fix64_t arg) {
    return (fix64_t){ (arg.repr >> FIX64_FRAC_BITS) << FIX64_FRAC_BITS };
}

/// Rounds up to the nearest integral value greater than or equal to the fixed point argument
///
/// @param arg the fixed point number to ceil
/// @return the ceil of arg
static inline fix64_t fix64_ceil(fix64_t arg) {
    // Add use floor and add (1 - eps) for rounding up
    return fix64_floor(fix64_add(arg, fix64_sub(FIX64_ONE, FIX64_EPSILON)));
}

/// Rounds the fixed point argument to the nearest integral value. Halfway values round away from
/// zero
///
/// @param arg the fixed point number to round
/// @return the rounded value
static inline fix64_t fix64_round(fix64_t arg) {
    if (fix64_lt(arg, FIX64_ZERO)) {
        return fix64_floor(fix64_add(arg, fix64_sub(FIX64_HALF, FIX64_EPSILON)));
    } else {
        return fix64_floor(fix64_add(arg, FIX64_HALF));
    }
}

/// Rounds the fixed point number towards zero
///
/// @param arg the fixed point number to truncate
/// @return the truncated value
static inline fix64_t fix64_trunc(fix64_t arg) {
    if (fix64_lt(arg, FIX64_ZERO)) {
        return fix64_ceil(arg);
    } else {
        return fix64_floor(arg);
    }
}

/// Absolute value of a fix64_t number. Note: fix64_abs(FIX64_MIN) returns FIX64_MAX.
///
/// @param arg fixed point number
/// @return absolute value of arg
static inline fix64_t fix64_abs(fix64_t arg) {
    if (fix64_lt(arg, FIX64_ZERO)) {
        return fix64_neg(arg);
    } else {
        return arg;
    }
}

/// Returns the greater of two fix64_t numbers
///
/// @param x a fixed point number
/// @param y the other fixed point number
/// @return the greater of x and y
static inline fix64_t fix64_max(fix64_t x, fix64_t y) {
    return fix64_gt(x, y) ? x : y;
}

/// Returns the lesser of two fix64_t numbers
///
/// @param x a fixed point number
/// @param y the other fixed point number
/// @return the lesser of x and y
static inline fix64_t fix64_min(fix64_t x, fix64_t y) {
    return fix64_lt(x, y) ? x : y;
}

/// Returns the absolute value of the difference between two numbers. The difference will saturate
/// at FIX64_MAX
///
/// @param x a fixed point number
/// @param y the other fixed point number
/// @return the absolute difference
static inline fix64_t fix64_dim(fix64_t x, fix64_t y) {
    return fix64_abs(fix64_sub_sat(x, y));
}

//==========================================================
// Exponential functions
//==========================================================

/// Returns e raised to the given power
///
/// @param arg fixed point number
/// @return e raised to the given power
fix64_t fix64_exp(fix64_t arg);

/// Returns 2 raised to the given power
///
/// @param arg fixed point number
/// @return 2 raised to the given power
fix64_t fix64_exp2(fix64_t arg);

/// Returns e raised to the given power, minus one
///
/// @param arg fixed point number
/// @return e raised to the given power, minus one
static inline fix64_t fix64_expm1(fix64_t arg) {
    return fix64_sub(fix64_exp(arg), FIX64_ONE);
}

/// Returns natural (base e) logarithm of a number
///
/// @param arg fixed point number
/// @return the natural logarithm
fix64_t fix64_log(fix64_t arg);

/// Returns base 10 logarithm of a number
///
/// @param arg fixed point number
/// @return the base 10 logarithm
fix64_t fix64_log10(fix64_t arg);

/// Returns base 2 logarithm of a number
///
/// @param arg fixed point number
/// @return the base 2 logarithm
fix64_t fix64_log2(fix64_t arg);

/// Returns natural (base e) logarithm of 1 plus the argument
///
/// @param arg fixed point number
/// @return the natural logarithm of 1 plus the argument
static inline fix64_t fix64_log1p(fix64_t arg) {
    return fix64_log(fix64_add_sat(FIX64_ONE, arg));
}

//==========================================================
// Power functions
//==========================================================

/// Raises a fixed point number to the power of another
///
/// @param x the base
/// @param y the exponent
/// @return the base raised to the power of the exponent
static inline fix64_t fix64_pow(fix64_t x, fix64_t y) {
    // TODO do this at higher precision?
    return fix64_exp2(fix64_mul_sat(y, fix64_log2(x)));
}

/// Calculates a number's square root
///
/// @param arg fixed point number
/// @return the square root
static inline fix64_t fix64_sqrt(fix64_t arg) {
    // TODO do this at higher precision?
    return fix64_pow(arg, FIX64_HALF);
}

/// Calculates a number's cube root
///
/// @param arg fixed point number
/// @return the cube root
static inline fix64_t fix64_cbrt(fix64_t arg) {
    // TODO do this at higher precision?
    return fix64_pow(arg, fix64_div(FIX64_ONE, FIX64_C(3.0)));
}

/// Calculates the hypotenuse of a right-angled triangle given the two other sides
///
/// @param x one side of the triangle
/// @param y the other side of the triangle
/// @return the length of the hypotenuse
static inline fix64_t fix64_hypot(fix64_t x, fix64_t y) {
    // TODO do this at higher precision?
    return fix64_sqrt(fix64_add_sat(fix64_mul_sat(x, x), fix64_mul_sat(y, y)));
}

//==========================================================
// Trigonometric functions
//==========================================================

/// Computes the sine of a given angle. The result is extremely accurate, with a maximum error of
/// +/-FIX64_EPSILON for <0.01% of inputs.
///
/// @param angle the angle
/// @return the sine of the angle
fix64_t fix64_sin(fix64_t angle);

/// Computes the cosine of a given angle. The result is extremely accurate, with a maximum error of
/// +/-FIX64_EPSILON for <0.01% of inputs.
///
/// @param angle the angle
/// @return the cosine of the angle
fix64_t fix64_cos(fix64_t angle);

/// Computes the tangent of a given angle. The result is extremely accurate, with a maximum error
/// of +/-FIX64_EPSILON for <0.01% of angles >0.0001pi from a singularity. Near the singularities
/// at +/-pi/2 the absolute error increases. The relative error remains small (less than 1e-12).
/// Extremely large results are saturated at FIX64_MAX or FIX64_MIN depending on the sign.
///
/// @param angle the angle
/// @return the tangent of the angle
fix64_t fix64_tan(fix64_t angle);

/// Computes arc sine of a given number
///
/// @param arg fixed point number
/// @return the arc sine
fix64_t fix64_asin(fix64_t arg);

/// Computes arc cosine of a given number
///
/// @param arg fixed point number
/// @return the arc cosine
fix64_t fix64_acos(fix64_t arg);

/// Computes 2 argument arc tangent of a given pair of numbers
///
/// @param x the horizontal component
/// @param y the vertical component
/// @return the arc tangent of y/x, taking their signs into account
fix64_t fix64_atan2(fix64_t x, fix64_t y);

/// Computes arc tangent of a given number
///
/// @param arg fixed point number
/// @return the arc tangent
static inline fix64_t fix64_atan(fix64_t arg) {
    return fix64_atan2(arg, FIX64_ONE);
}

//==========================================================
// Error and Gamma functions
//==========================================================

/// computes error function
///
/// @param arg a fixed point number
/// @return the result of the error function
fix64_t fix64_erf(fix64_t arg);

/// computes complementary error function
///
/// @param arg a fixed point number
/// @return the result of the complementary error function
fix64_t fix64_erfc(fix64_t arg);

/// computes gamma function
///
/// @param arg a fixed point number
/// @return the result of the gamma function
fix64_t fix64_tgamma(fix64_t arg);

/// computes natural logarithm of the gamma function
///
/// @param arg a fixed point number
/// @return the natural logarithm of the result of the gamma function
fix64_t fix64_lgamma(fix64_t arg);
