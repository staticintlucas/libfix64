#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/// Signed fixed point Q31.32 type
typedef struct {
    int64_t repr;
} fix64_t;

/// Number of fractional bits for fix64_t
#define FIX64_FRAC_BITS 32
/// Number of integral bits for fix64_t (excluding sign bit)
#define FIX64_INT_BITS  31
/// Number of significant bits for fix64_t (excluding sign bit)
#define FIX64_BITS      (FIX64_INT_BITS + FIX64_FRAC_BITS)
/// Maximum value for a fix64_t
#define FIX64_MAX       ((fix64_t){ INT64_MAX })
/// Minimum value for a fix64_t
#define FIX64_MIN       ((fix64_t){ INT64_MIN })
/// Smallest positive non-zero value for fix64_t
#define FIX64_EPSILON   ((fix64_t){ INT64_C(1) })

// clang-format tries to wrap inside a type cast (ew...)
// clang-format off
/// Creates a fix64_t literal, for example FIX64_C(2.5)
#define FIX64_C(x)                                                          \
    ((fix64_t){ (int64_t)((long double)(x##L) * (1ull << FIX64_FRAC_BITS) + \
    (x##L < 0.L ? -0.5L : 0.5L)) })
// clang-format on

#include "fix64/arith.h"
#include "fix64/cmp.h"
#include "fix64/consts.h"
#include "fix64/cvt.h"
#include "fix64/math.h"
#include "fix64/str.h"

#ifdef __cplusplus
}
#endif
