#pragma once

#include <float.h>
#include <math.h>
#include <stdint.h>

#ifndef __has_builtin
    #define __has_builtin(_) 0
#endif
#if __has_builtin(__builtin_expect_with_probability)
    // Use probability of 0.9 - this is high enough that clang will reorder branches, etc. but it
    // won't use branches instead of cmov instructions (it does this with probability >0.99, default
    // for __builtin_expect seems to be 1.0). GCC uses default of probability of 0.9 and uses cmov
    // anyway regardless of probability
    #define FIX64_LIKELY(cond) __builtin_expect_with_probability((cond), 1, 0.9)
    #define FIX64_UNLIKELY(cond) __builtin_expect_with_probability((cond), 0, 0.9)
#elif __has_builtin(__builtin_expect)
    #define FIX64_LIKELY(cond) __builtin_expect((cond), 1)
    #define FIX64_UNLIKELY(cond) __builtin_expect((cond), 0)
#else
    #define FIX64_LIKELY(cond) (cond)
    #define FIX64_UNLIKELY(cond) (cond)
#endif

//==========================================================
// Type definitions
//==========================================================

/// Unsigned fixed point uq32.32 type
typedef struct {
    uint64_t repr;
} ufix64_t;

/// Signed fixed point q31.32 type
typedef struct {
    int64_t repr;
} fix64_t;

//==========================================================
// Defines
//==========================================================

#define UFIX64_FRAC_BITS 32 ///< Number of fractional bits for ::ufix64_t
#define UFIX64_INT_BITS 32 ///< Number of integral bits for ::ufix64_t
#define UFIX64_MAX ((ufix64_t){UINT64_MAX}) ///< Maximum value for a ::ufix64_t
#define UFIX64_MIN ((ufix64_t){0}) ///< Minimum value for a ::ufix64_t
#define UFIX64_EPSILON ((ufix64_t){1}) ///< Smallest positive non-zero value for ::ufix64_t

#define FIX64_FRAC_BITS 32 ///< Number of fractional bits for ::fix64_t
#define FIX64_INT_BITS 31 ///< Number of integral bits for ::fix64_t (excluding sign bit)
#define FIX64_MAX ((fix64_t){INT64_MAX}) ///< Maximum value for a ::fix64_t
#define FIX64_MIN ((fix64_t){INT64_MIN}) ///< Minimum value for a ::fix64_t
#define FIX64_EPSILON ((fix64_t){1}) ///< Smallest positive non-zero value for ::fix64_t

//==========================================================
// Literals
//==========================================================

// These function calls will be inlined by the compiler, Clang 13 does this even with -Og

/// Creates a ufix64_t literal, for example `UFIX64_C(2.5)`
#define UFIX64_C(x) (ufix64_from_ldbl((long double)(x##L)))
/// Creates a fix64_t literal, for example `FIX64_C(2.5)`
#define FIX64_C(x) (fix64_from_ldbl((long double)(x##L)))

//==========================================================
// Constants
//==========================================================

/// ufix64_t constant value 0
#define UFIX64_ZERO ((ufix64_t){UINT64_C(0x0)})
/// ufix64_t constant value 1
#define UFIX64_ONE ((ufix64_t){UINT64_C(0x100000000)})
/// ufix64_t constant value 0.5
#define UFIX64_HALF ((ufix64_t){UINT64_C(0x80000000)})
/// ufix64_t constant value 2
#define UFIX64_TWO ((ufix64_t){UINT64_C(0x200000000)})

/// ufix64_t mathematical constant e
#define UFIX64_E ((ufix64_t){UINT64_C(0x2b7e15163)})
/// ufix64_t mathematical constant log_2(e)
#define UFIX64_LOG2E ((ufix64_t){UINT64_C(0x171547653)})
/// ufix64_t mathematical constant log_10(e)
#define UFIX64_LOG10E ((ufix64_t){UINT64_C(0x06f2dec55)})
/// ufix64_t mathematical constant ln(2)
#define UFIX64_LN2 ((ufix64_t){UINT64_C(0x0b17217f8)})
/// ufix64_t mathematical constant ln(10)
#define UFIX64_LN10 ((ufix64_t){UINT64_C(0x24d763777)})
/// ufix64_t mathematical constant pi
#define UFIX64_PI ((ufix64_t){UINT64_C(0x3243f6a89)})
/// ufix64_t mathematical constant pi/2
#define UFIX64_PI_2 ((ufix64_t){UINT64_C(0x1921fb544)})
/// ufix64_t mathematical constant pi/4
#define UFIX64_PI_4 ((ufix64_t){UINT64_C(0x0c90fdaa2)})
/// ufix64_t mathematical constant 1/pi
#define UFIX64_1_PI ((ufix64_t){UINT64_C(0x0517cc1b7)})
/// ufix64_t mathematical constant 2/pi
#define UFIX64_2_PI ((ufix64_t){UINT64_C(0x0a2f9836e)})
/// ufix64_t mathematical constant 2/sqrt(pi)
#define UFIX64_2_SQRTPI ((ufix64_t){UINT64_C(0x120dd7504)})
/// ufix64_t mathematical constant sqrt(2)
#define UFIX64_SQRT2 ((ufix64_t){UINT64_C(0x16a09e668)})
/// ufix64_t mathematical constant sqrt(1/2)
#define UFIX64_SQRT1_2 ((ufix64_t){UINT64_C(0x0b504f334)})

/// ufix64_t constant value 0
#define FIX64_ZERO ((fix64_t){INT64_C(0x0)})
/// ufix64_t constant value 1
#define FIX64_ONE ((fix64_t){INT64_C(0x100000000)})
/// ufix64_t constant value 0.5
#define FIX64_HALF ((fix64_t){INT64_C(0x80000000)})
/// ufix64_t constant value 2
#define FIX64_TWO ((fix64_t){INT64_C(0x200000000)})

/// fix64_t mathematical constant e
#define FIX64_E ((fix64_t){INT64_C(0x2b7e15163)})
/// fix64_t mathematical constant log_2(e)
#define FIX64_LOG2E ((fix64_t){INT64_C(0x171547653)})
/// fix64_t mathematical constant log_10(e)
#define FIX64_LOG10E ((fix64_t){INT64_C(0x06f2dec55)})
/// fix64_t mathematical constant ln(2)
#define FIX64_LN2 ((fix64_t){INT64_C(0x0b17217f8)})
/// fix64_t mathematical constant ln(10)
#define FIX64_LN10 ((fix64_t){INT64_C(0x24d763777)})
/// fix64_t mathematical constant pi
#define FIX64_PI ((fix64_t){INT64_C(0x3243f6a89)})
/// fix64_t mathematical constant pi/2
#define FIX64_PI_2 ((fix64_t){INT64_C(0x1921fb544)})
/// fix64_t mathematical constant pi/4
#define FIX64_PI_4 ((fix64_t){INT64_C(0x0c90fdaa2)})
/// fix64_t mathematical constant 1/pi
#define FIX64_1_PI ((fix64_t){INT64_C(0x0517cc1b7)})
/// fix64_t mathematical constant 2/pi
#define FIX64_2_PI ((fix64_t){INT64_C(0x0a2f9836e)})
/// fix64_t mathematical constant 2/sqrt(pi)
#define FIX64_2_SQRTPI ((fix64_t){INT64_C(0x120dd7504)})
/// fix64_t mathematical constant sqrt(2)
#define FIX64_SQRT2 ((fix64_t){INT64_C(0x16a09e668)})
/// fix64_t mathematical constant sqrt(1/2)
#define FIX64_SQRT1_2 ((fix64_t){INT64_C(0x0b504f334)})

//==========================================================
// Generic functions
//==========================================================

// clang-format doesn't handle _Generic well, disable it for this section
// clang-format off

/// Generic macro to convert a fixed point type to `float`
#define fix_to_flt(x)            \
    _Generic((x),                \
        ufix64_t: ufix64_to_flt, \
        fix64_t: fix64_to_flt,   \
    )(x)

/// Generic macro to convert a fixed point type to `double`
#define fix_to_dbl(x)            \
    _Generic((x),                \
        ufix64_t: ufix64_to_dbl, \
        fix64_t: fix64_to_dbl,   \
    )(x)

/// Generic macro to convert a fixed point type to `long double`
#define fix_to_ldbl(x)            \
    _Generic((x),                 \
        ufix64_t: ufix64_to_ldbl, \
        fix64_t: fix64_to_ldbl,   \
    )(x)

/// Generic macro to convert a floating point type to ::ufix64_t
#define ufix64_from(x)                 \
    _Generic((x),                      \
        float: ufix64_from_flt,        \
        double: ufix64_from_dbl,       \
        long double: ufix64_from_ldbl, \
    )(x)

/// Generic macro to convert a floating point type to ::fix64_t
#define fix64_from(x)                 \
    _Generic((x),                     \
        float: fix64_from_flt,        \
        double: fix64_from_dbl,       \
        long double: fix64_from_ldbl, \
    )(x)

// clang-format on

//==========================================================
// Conversion functions
//==========================================================

/// Converts a ::ufix64_t to `float`
///
/// @param value the value to convert
/// @return equivalent `float` value
static inline float ufix64_to_flt(ufix64_t value);

/// Converts a ::ufix64_t to `float`
///
/// @param value the value to convert
/// @return equivalent `float` value
static inline float fix64_to_flt(fix64_t value);

/// Converts a ::ufix64_t to `double`
///
/// @param value the value to convert
/// @return equivalent `double` value
static inline double ufix64_to_dbl(ufix64_t value);

/// Converts a ::fix64_t to `double`
///
/// @param value the value to convert
/// @return equivalent `double` value
static inline double fix64_to_dbl(fix64_t value);

/// Converts a ::ufix64_t to `long double`
///
/// @param value the value to convert
/// @return equivalent `long double` value
static inline long double ufix64_to_ldbl(ufix64_t value);

/// Converts a ::fix64_t to `long double`
///
/// @param value the value to convert
/// @return equivalent `long double` value
static inline long double fix64_to_ldbl(fix64_t value);

/// Converts a `float` to ::ufix64_t
///
/// Saturates if input is outside of ::ufix64_t's range. Undefined behaviour if input is NaN
///
/// @param value the value to convert
/// @return equivalent ::ufix64_t value
static inline ufix64_t ufix64_from_flt(float value);

/// Converts a `double` to ::ufix64_t
///
/// Saturates if input is outside of ::ufix64_t's range. Undefined behaviour if input is NaN
///
/// @param value the value to convert
/// @return equivalent ::ufix64_t value
static inline ufix64_t ufix64_from_dbl(double value);

/// Converts a `long double` to ::ufix64_t
///
/// Saturates if input is outside of ::ufix64_t's range. Undefined behaviour if input is NaN
///
/// @param value the value to convert
/// @return equivalent ::ufix64_t value
static inline ufix64_t ufix64_from_ldbl(long double value);

/// Converts a `float` to ::fix64_t
///
/// Saturates if input is outside of ::fix64_t's range. Undefined behaviour if input is NaN
///
/// @param value the value to convert
/// @return equivalent ::fix64_t value
static inline fix64_t fix64_from_flt(float value);

/// Converts a `double` to ::fix64_t
///
/// Saturates if input is outside of ::fix64_t's range. Undefined behaviour if input is NaN
///
/// @param value the value to convert
/// @return equivalent ::fix64_t value
static inline fix64_t fix64_from_dbl(double value);

/// Converts a `long double` to ::fix64_t
///
/// Saturates if input is outside of ::fix64_t's range. Undefined behaviour if input is NaN
///
/// @param value the value to convert
/// @return equivalent ::fix64_t value
static inline fix64_t fix64_from_ldbl(long double value);

//==========================================================
// Comparison operations
//==========================================================

/// Compares two ::ufix64_t values for equality
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if they are equal, `0` otherwise
static inline int ufix64_eq(ufix64_t lhs, ufix64_t rhs);

/// Compares two ::ufix64_t values for inequality
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `0` if they are equal, `1` otherwise
static inline int ufix64_neq(ufix64_t lhs, ufix64_t rhs);

/// Check if a ::ufix64_t is less than another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is less than `rhs`, `0` otherwise
static inline int ufix64_lt(ufix64_t lhs, ufix64_t rhs);

/// Check if a ::ufix64_t is greater than another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is greater than `rhs`, `0` otherwise
static inline int ufix64_gt(ufix64_t lhs, ufix64_t rhs);

/// Check if a ::ufix64_t is less than or equal to another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is less than or equal to `rhs`, `0` otherwise
static inline int ufix64_lte(ufix64_t lhs, ufix64_t rhs);

/// Check if a ::ufix64_t is greater than or equal to another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is greater than or equal to `rhs`, `0` otherwise
static inline int ufix64_gte(ufix64_t lhs, ufix64_t rhs);

/// Compares two ::fix64_t values for equality
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if they are equal, `0` otherwise
static inline int fix64_eq(fix64_t lhs, fix64_t rhs);

/// Compares two ::fix64_t values for inequality
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `0` if they are equal, `1` otherwise
static inline int fix64_neq(fix64_t lhs, fix64_t rhs);

/// Check if a ::fix64_t is less than another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is less than `rhs`, `0` otherwise
static inline int fix64_lt(fix64_t lhs, fix64_t rhs);

/// Check if a ::fix64_t is greater than another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is greater than `rhs`, `0` otherwise
static inline int fix64_gt(fix64_t lhs, fix64_t rhs);

/// Check if a ::fix64_t is less than or equal to another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is less than or equal to `rhs`, `0` otherwise
static inline int fix64_lte(fix64_t lhs, fix64_t rhs);

/// Check if a ::fix64_t is greater than or equal to another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return `1` if `lhs` is greater than or equal to `rhs`, `0` otherwise
static inline int fix64_gte(fix64_t lhs, fix64_t rhs);

//==========================================================
// Arithmetic operations
//==========================================================

/// Addition of two ::ufix64_t numbers
///
/// @param lhs left hand side for the addition
/// @param rhs right hand side for the addition
/// @return the sum of the two inputs
static inline ufix64_t ufix64_add(ufix64_t lhs, ufix64_t rhs);

/// Saturating addition of two ::ufix64_t numbers
///
/// @param lhs left hand side for the addition
/// @param rhs right hand side for the addition
/// @return the sum of the two inputs
static inline ufix64_t ufix64_add_sat(ufix64_t lhs, ufix64_t rhs);

/// Subtraction of two ::ufix64_t numbers
///
/// @param lhs left hand side for the subtraction
/// @param rhs right hand side for the subtraction
/// @return the difference of the two inputs
static inline ufix64_t ufix64_sub(ufix64_t lhs, ufix64_t rhs);

/// Saturating subtraction of two ::ufix64_t numbers
///
/// @param lhs left hand side for the subtraction
/// @param rhs right hand side for the subtraction
/// @return the difference of the two inputs
static inline ufix64_t ufix64_sub_sat(ufix64_t lhs, ufix64_t rhs);

/// Multiplication of two ::ufix64_t numbers
///
/// @param lhs left hand side for the multiplication
/// @param rhs right hand side for the multiplication
/// @return the product of the two inputs
static inline ufix64_t ufix64_mul(ufix64_t lhs, ufix64_t rhs);

/// Saturating multiplication of two ::ufix64_t numbers
///
/// @param lhs left hand side for the multiplication
/// @param rhs right hand side for the multiplication
/// @return the product of the two inputs
static inline ufix64_t ufix64_mul_sat(ufix64_t lhs, ufix64_t rhs);

/// Addition of two ::fix64_t numbers
///
/// @param lhs left hand side for the addition
/// @param rhs right hand side for the addition
/// @return the sum of the two inputs
static inline fix64_t fix64_add(fix64_t lhs, fix64_t rhs);

/// Saturating addition of two ::fix64_t numbers
///
/// @param lhs left hand side for the addition
/// @param rhs right hand side for the addition
/// @return the sum of the two inputs
static inline fix64_t fix64_add_sat(fix64_t lhs, fix64_t rhs);

/// Subtraction of two ::fix64_t numbers
///
/// @param lhs left hand side for the subtraction
/// @param rhs right hand side for the subtraction
/// @return the difference of the two inputs
static inline fix64_t fix64_sub(fix64_t lhs, fix64_t rhs);

/// Saturating subtraction of two ::fix64_t numbers
///
/// @param lhs left hand side for the subtraction
/// @param rhs right hand side for the subtraction
/// @return the difference of the two inputs
static inline fix64_t fix64_sub_sat(fix64_t lhs, fix64_t rhs);

/// Multiplication of two ::fix64_t numbers
///
/// @param lhs left hand side for the multiplication
/// @param rhs right hand side for the multiplication
/// @return the product of the two inputs
static inline fix64_t fix64_mul(fix64_t lhs, fix64_t rhs);

/// Saturating multiplication of two ::fix64_t numbers
///
/// @param lhs left hand side for the multiplication
/// @param rhs right hand side for the multiplication
/// @return the product of the two inputs
static inline fix64_t fix64_mul_sat(fix64_t lhs, fix64_t rhs);

//==========================================================
// Conversion function implementations
//==========================================================

static inline float ufix64_to_flt(ufix64_t value) {
    return ((float)(value.repr) / (float)(UINT64_C(1) << UFIX64_FRAC_BITS));
}

static inline double ufix64_to_dbl(ufix64_t value) {
    return ((double)(value.repr) / (double)(UINT64_C(1) << UFIX64_FRAC_BITS));
}

static inline long double ufix64_to_ldbl(ufix64_t value) {
    return ((long double)(value.repr) / (long double)(UINT64_C(1) << UFIX64_FRAC_BITS));
}

static inline float fix64_to_flt(fix64_t value) {
    return ((float)(value.repr) / (float)(UINT64_C(1) << FIX64_FRAC_BITS));
}

static inline double fix64_to_dbl(fix64_t value) {
    return ((double)(value.repr) / (double)(UINT64_C(1) << FIX64_FRAC_BITS));
}

static inline long double fix64_to_ldbl(fix64_t value) {
    return ((long double)(value.repr) / (long double)(UINT64_C(1) << FIX64_FRAC_BITS));
}

// Note: for (sort-of) efficient saturating floating point to fixed conversions, we rely on these
// integer type limits converted to float and rounded towards zero. These are used instead of
// fesetround since certain compilers like to reorder casts compared to those function calls so they
// have no effect. These macros are set by zeroing the LSBs so that the integral max has only as
// many digits as the floating point mantissa so the exact value is preserved when casting. The
// signed MIN values are just the negative of the max since calculating the actual minimum is more
// difficult, and at worst we're one interval off. We compensate for this by only comparing < MIN;
// values == MIN are converted as usual so if MIN is off by one the results still won't be affected.
#if FLT_RADIX == 2
    #if FLT_MANT_DIG >= 64
        #define MAX_FLT_UINT64 ((float)UINT64_MAX)
    #else
        #define MAX_FLT_UINT64 ((float)((UINT64_MAX) & ~((UINT64_C(1) << (64 - FLT_MANT_DIG)) - 1)))
    #endif
    #if FLT_MANT_DIG >= 63
        #define MAX_FLT_INT64 ((float)INT64_MAX)
        #define MIN_FLT_INT64 ((float)INT64_MIN)
    #else
        #define MAX_FLT_INT64 ((float)((INT64_MAX) & ~((UINT64_C(1) << (63 - FLT_MANT_DIG)) - 1)))
        #define MIN_FLT_INT64 (-MAX_FLT_INT64)
    #endif

    #if DBL_MANT_DIG >= 64
        #define MAX_DBL_UINT64 ((double)UINT64_MAX)
    #else
        #define MAX_DBL_UINT64 \
            ((double)((UINT64_MAX) & ~((UINT64_C(1) << (64 - DBL_MANT_DIG)) - 1)))
    #endif
    #if DBL_MANT_DIG >= 63
        #define MAX_DBL_INT64 ((double)INT64_MAX)
        #define MIN_DBL_INT64 ((double)INT64_MIN)
    #else
        #define MAX_DBL_INT64 ((double)((INT64_MAX) & ~((UINT64_C(1) << (63 - DBL_MANT_DIG)) - 1)))
        #define MIN_DBL_INT64 (-MAX_DBL_INT64)
    #endif

    #if LDBL_MANT_DIG >= 64
        #define MAX_LDBL_UINT64 ((long double)UINT64_MAX)
    #else
        #define MAX_LDBL_UINT64 \
            ((long double)((UINT64_MAX) & ~((UINT64_C(1) << (64 - LDBL_MANT_DIG)) - 1)))
    #endif
    #if LDBL_MANT_DIG >= 63
        #define MAX_LDBL_INT64 ((long double)INT64_MAX)
        #define MIN_LDBL_INT64 ((long double)INT64_MIN)
    #else
        #define MAX_LDBL_INT64 \
            ((long double)((INT64_MAX) & ~((UINT64_C(1) << (63 - LDBL_MANT_DIG)) - 1)))
        #define MIN_LDBL_INT64 (-MAX_LDBL_INT64)
    #endif
#else
    #error "FLT_RADIX != 2 not currently supported!"
#endif

static inline ufix64_t ufix64_from_flt(float value) {
    uint64_t repr;

    // NOLINTNEXTLINE(readability-magic-numbers) add 0.5 for rounding to uint64_t
    value = value * exp2f(UFIX64_FRAC_BITS) + 0.5f;

    if (FIX64_UNLIKELY(value > MAX_FLT_UINT64)) {
        repr = UINT64_MAX;
    } else if (FIX64_UNLIKELY(value < 0.0f)) {
        repr = 0;
    } else {
        repr = (uint64_t)value;
    }

    return (ufix64_t){repr};
}

static inline ufix64_t ufix64_from_dbl(double value) {
    uint64_t repr;

    // NOLINTNEXTLINE(readability-magic-numbers) add 0.5 for rounding to uint64_t
    value = value * exp2(UFIX64_FRAC_BITS) + 0.5;

    if (FIX64_UNLIKELY(value > MAX_DBL_UINT64)) {
        repr = UINT64_MAX;
    } else if (FIX64_UNLIKELY(value < 0.0)) {
        repr = 0;
    } else {
        repr = (uint64_t)value;
    }

    return (ufix64_t){repr};
}

static inline ufix64_t ufix64_from_ldbl(long double value) {
    uint64_t repr;

    // Note: clang fails to optimise out call to exp2l, so we use a bit shift instead. A long
    // double is more than large enough to store the result so the compiler doesn't complain
    // like it does with float.
    // NOLINTNEXTLINE(readability-magic-numbers) add 0.5 for rounding to uint64_t
    value = value * (UINT64_C(1) << UFIX64_FRAC_BITS) + 0.5l;

    if (FIX64_UNLIKELY(value > MAX_LDBL_UINT64)) {
        repr = UINT64_MAX;
    } else if (FIX64_UNLIKELY(value < 0.0l)) {
        repr = 0;
    } else {
        repr = (uint64_t)value;
    }

    return (ufix64_t){repr};
}

static inline fix64_t fix64_from_flt(float value) {
    int64_t repr;

    // NOLINTNEXTLINE(readability-magic-numbers) add 0.5 for rounding to uint64_t
    value = value * exp2f(FIX64_FRAC_BITS) + copysignf(0.5f, value);

    if (FIX64_UNLIKELY(value > MAX_FLT_INT64)) {
        repr = INT64_MAX;
    } else if (FIX64_UNLIKELY(value < MIN_FLT_INT64)) {
        repr = INT64_MIN;
    } else {
        repr = (int64_t)value;
    }

    return (fix64_t){repr};
}

static inline fix64_t fix64_from_dbl(double value) {
    int64_t repr;

    // NOLINTNEXTLINE(readability-magic-numbers) add 0.5 for rounding to uint64_t
    value = value * exp2(FIX64_FRAC_BITS) + copysign(0.5, value);

    if (FIX64_UNLIKELY(value > MAX_DBL_INT64)) {
        repr = INT64_MAX;
    } else if (FIX64_UNLIKELY(value < MIN_DBL_INT64)) {
        repr = INT64_MIN;
    } else {
        repr = (int64_t)value;
    }

    return (fix64_t){repr};
}

static inline fix64_t fix64_from_ldbl(long double value) {
    int64_t repr;

    // Note: clang fails to optimise out call to exp2l, so we use a bit shift instead. A long
    // double is more than large enough to store the result so the compiler doesn't complain
    // like it does with float.
    // NOLINTNEXTLINE(readability-magic-numbers) add 0.5 for rounding to uint64_t
    value = value * (UINT64_C(1) << UFIX64_FRAC_BITS) + copysignl(0.5l, value);

    if (FIX64_UNLIKELY(value > MAX_LDBL_INT64)) {
        repr = INT64_MAX;
    } else if (FIX64_UNLIKELY(value < MIN_LDBL_INT64)) {
        repr = INT64_MIN;
    } else {
        repr = (int64_t)value;
    }

    return (fix64_t){repr};
}

#undef MAX_LDBL_UINT64
#undef MAX_LDBL_INT64
#undef MIN_LDBL_INT64
#undef MAX_DBL_UINT64
#undef MAX_DBL_INT64
#undef MIN_DBL_INT64
#undef MAX_FLT_UINT64
#undef MAX_FLT_INT64
#undef MIN_FLT_INT64

//==========================================================
// Comparison operation implementations
//==========================================================

static inline int ufix64_eq(ufix64_t lhs, ufix64_t rhs) {
    return (lhs.repr == rhs.repr);
}

static inline int ufix64_neq(ufix64_t lhs, ufix64_t rhs) {
    return (lhs.repr != rhs.repr);
}

static inline int ufix64_lt(ufix64_t lhs, ufix64_t rhs) {
    return (lhs.repr < rhs.repr);
}

static inline int ufix64_gt(ufix64_t lhs, ufix64_t rhs) {
    return (lhs.repr > rhs.repr);
}

static inline int ufix64_lte(ufix64_t lhs, ufix64_t rhs) {
    return (lhs.repr <= rhs.repr);
}

static inline int ufix64_gte(ufix64_t lhs, ufix64_t rhs) {
    return (lhs.repr >= rhs.repr);
}

static inline int fix64_eq(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr == rhs.repr);
}

static inline int fix64_neq(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr != rhs.repr);
}

static inline int fix64_lt(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr < rhs.repr);
}

static inline int fix64_gt(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr > rhs.repr);
}

static inline int fix64_lte(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr <= rhs.repr);
}

static inline int fix64_gte(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr >= rhs.repr);
}

//==========================================================
// Arithmetic operation implementations
//==========================================================

static inline ufix64_t ufix64_add(ufix64_t lhs, ufix64_t rhs) {
    return (ufix64_t){.repr = lhs.repr + rhs.repr};
}

static inline ufix64_t ufix64_add_sat(ufix64_t lhs, ufix64_t rhs) {
    ufix64_t result = ufix64_add(lhs, rhs);
    if (FIX64_UNLIKELY(ufix64_gt(lhs, result))) {
        result.repr = UINT64_MAX;
    }
    return result;
}

static inline ufix64_t ufix64_sub(ufix64_t lhs, ufix64_t rhs) {
    return (ufix64_t){.repr = lhs.repr - rhs.repr};
}

static inline ufix64_t ufix64_sub_sat(ufix64_t lhs, ufix64_t rhs) {
    ufix64_t result = ufix64_sub(lhs, rhs);
    if (FIX64_UNLIKELY(ufix64_lt(lhs, result))) {
        result.repr = 0;
    }
    return result;
}

static inline ufix64_t ufix64_mul(ufix64_t lhs, ufix64_t rhs) {
    __uint128_t result = ((__uint128_t)lhs.repr * rhs.repr);
    result += (UINT64_C(1) << (UFIX64_FRAC_BITS - 1)); // add 0x80000000 for rounding
    result >>= UFIX64_FRAC_BITS;
    return (ufix64_t){(uint64_t)result};
}

static inline ufix64_t ufix64_mul_sat(ufix64_t lhs, ufix64_t rhs) {
    __uint128_t result = ((__uint128_t)lhs.repr * rhs.repr);
    result += (UINT64_C(1) << (UFIX64_FRAC_BITS - 1)); // add 0x80000000 for rounding
    result >>= UFIX64_FRAC_BITS;
    if (FIX64_UNLIKELY(result > (__uint128_t)UINT64_MAX)) {
        result = UINT64_MAX;
    }
    return (ufix64_t){(uint64_t)result};
}

static inline fix64_t fix64_add(fix64_t lhs, fix64_t rhs) {
    return (fix64_t){lhs.repr + rhs.repr};
}

static inline fix64_t fix64_add_sat(fix64_t lhs, fix64_t rhs) {
    int64_t result;
    // TODO add compiler-agnostic version? Compilers currently produce pretty suboptimal signed
    // saturation code for any implementation that doesn't use __builtin_*_overflow
    int overflow = __builtin_add_overflow(lhs.repr, rhs.repr, &result);
    if (FIX64_UNLIKELY(overflow)) {
        result = (rhs.repr < 0) ? INT64_MIN : INT64_MAX;
    }
    return (fix64_t){result};
}

static inline fix64_t fix64_sub(fix64_t lhs, fix64_t rhs) {
    return (fix64_t){lhs.repr - rhs.repr};
}

static inline fix64_t fix64_sub_sat(fix64_t lhs, fix64_t rhs) {
    int64_t result;
    // TODO add compiler-agnostic version? Compilers currently produce pretty suboptimal signed
    // saturation code for any implementation that doesn't use __builtin_*_overflow
    int overflow = __builtin_sub_overflow(lhs.repr, rhs.repr, &result);
    if (FIX64_UNLIKELY(overflow)) {
        result = (rhs.repr > 0) ? INT64_MIN : INT64_MAX;
    }
    return (fix64_t){result};
}

static inline fix64_t fix64_mul(fix64_t lhs, fix64_t rhs) {
    __int128_t result = ((__int128_t)lhs.repr * rhs.repr);
    result += (INT64_C(1) << (FIX64_FRAC_BITS - 1)); // add 0x80000000 for rounding
    result >>= FIX64_FRAC_BITS;
    return (fix64_t){(int64_t)result};
}

static inline fix64_t fix64_mul_sat(fix64_t lhs, fix64_t rhs) {
    __int128_t result = ((__int128_t)lhs.repr * rhs.repr);
    result += (INT64_C(1) << (FIX64_FRAC_BITS - 1)); // add 0x80000000 for rounding
    result >>= FIX64_FRAC_BITS;
    if (FIX64_UNLIKELY(result > (__int128_t)INT64_MAX)) {
        result = INT64_MAX;
    } else if (FIX64_UNLIKELY(result < (__int128_t)INT64_MIN)) {
        result = INT64_MIN;
    }
    return (fix64_t){(int64_t)result};
}

// Undefine internal defines
#undef FIX64_LIKELY
#undef FIX64_UNLIKELY
