#pragma once

#include <limits.h>
#include <stdint.h>

// Feature detection

#ifdef __has_builtin
    #define FIX64_IMPL_HAS_BUILTIN(builtin, gcc_ver) __has_builtin(builtin)
#elif defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
    // GCC < 10 didn't have __has_builtin, check the version number as a fallback
    #define FIX64_IMPL_HAS_BUILTIN(builtin, gcc_ver) (__GNUC__ >= gcc_ver)
#else
    #define FIX64_IMPL_HAS_BUILTIN(builtin, gcc_ver) 0
#endif

#if !defined(FIX64_IMPL_OVERRIDE_USE_FALLBACK)
    #if FIX64_IMPL_HAS_BUILTIN(__builtin_expect_with_probability, 9)
        #define FIX64_IMPL_USE_BUILTIN_EXPECT_WITH_PROBABILITY 1
    #elif FIX64_IMPL_HAS_BUILTIN(__builtin_expect, 3)
        #define FIX64_IMPL_USE_BUILTIN_EXPECT 1
    #endif
#endif

#if FIX64_IMPL_HAS_BUILTIN(__builtin_add_overflow, 5) && !defined(FIX64_IMPL_OVERRIDE_USE_FALLBACK)
    #define FIX64_IMPL_USE_BUILTIN_ARITH_OVERFLOW 1
#endif

#if defined(__SIZEOF_INT128__) && !defined(FIX64_IMPL_OVERRIDE_USE_FALLBACK)
    #define FIX64_IMPL_USE_INT128 1
#endif

#if defined(__x86_64__) && defined(__GNUC__) && !defined(FIX64_IMPL_OVERRIDE_USE_FALLBACK)
    #define FIX64_IMPL_USE_NATIVE_DIVQ 1
#endif

#if FIX64_IMPL_HAS_BUILTIN(__builtin_clz, 4) && !defined(FIX64_IMPL_OVERRIDE_USE_FALLBACK)
    #define FIX64_IMPL_USE_BUILTIN_CLZ 1
#endif

// Implement features

#if FIX64_IMPL_USE_BUILTIN_EXPECT_WITH_PROBABILITY
    // Use __builtin_expect_with_probability if available with a probability of 0.9. GCC already
    // uses 0.9 by default for __builtin_expect, but Clang uses 1.0. This makes Clang perform
    // optimisations such as replacing cmov with branches which make the expected case marginally
    // faster while making the other case significantly slower. To get around this we explicitly
    // use 0.9 probability which will reorder branches, etc, but otherwise won't go overboard.
    #define FIX64_LIKELY(cond)   __builtin_expect_with_probability((cond), 1, 0.9)
    #define FIX64_UNLIKELY(cond) __builtin_expect_with_probability((cond), 0, 0.9)
#elif FIX64_IMPL_USE_BUILTIN_EXPECT
    #define FIX64_LIKELY(cond)   __builtin_expect((cond), 1)
    #define FIX64_UNLIKELY(cond) __builtin_expect((cond), 0)
#else
    #define FIX64_LIKELY(cond)   (cond)
    #define FIX64_UNLIKELY(cond) (cond)
#endif

#if FIX64_IMPL_USE_BUILTIN_ARITH_OVERFLOW
static inline int fix64_impl_add_u64_overflow(uint64_t x, uint64_t y, uint64_t *result) {
    return __builtin_add_overflow(x, y, result);
}
static inline int fix64_impl_add_i64_overflow(int64_t x, int64_t y, int64_t *result) {
    return __builtin_add_overflow(x, y, result);
}
static inline int fix64_impl_sub_u64_underflow(uint64_t x, uint64_t y, uint64_t *result) {
    return __builtin_sub_overflow(x, y, result);
}
static inline int fix64_impl_sub_i64_underflow(int64_t x, int64_t y, int64_t *result) {
    return __builtin_sub_overflow(x, y, result);
}
#else
static inline int fix64_impl_add_u64_overflow(uint64_t x, uint64_t y, uint64_t *result) {
    *result = x + y;
    return *result < x;
}
static inline int fix64_impl_add_i64_overflow(int64_t x, int64_t y, int64_t *result) {
    uint64_t ures = (uint64_t)x + (uint64_t)y; // Avoid UB of signed overflow
    // Avoid UB of unsigned -> signed conversion, gets optimised out on most compilers
    *result = (ures > INT64_MAX) ? (int64_t)(ures - INT64_MIN) + INT64_MIN : (int64_t)ures;
    // If neither of the signs match the result there was an overflow
    return ((ures ^ (uint64_t)x) & (ures ^ (uint64_t)y)) >> 63;
}
static inline int fix64_impl_sub_u64_underflow(uint64_t x, uint64_t y, uint64_t *result) {
    *result = x - y;
    return *result > x;
}
static inline int fix64_impl_sub_i64_underflow(int64_t x, int64_t y, int64_t *result) {
    uint64_t ures = (uint64_t)x - (uint64_t)y; // Avoid UB of signed overflow
    // Avoid UB of unsigned -> signed conversion, gets optimised out on most compilers
    *result = (ures > INT64_MAX) ? (int64_t)(ures - INT64_MIN) + INT64_MIN : (int64_t)ures;
    // If neither result nor y have the same sign as x there was overflow
    return (((uint64_t)y ^ (uint64_t)x) & (ures ^ (uint64_t)x)) >> 63;
}
#endif

#if FIX64_IMPL_USE_INT128
static inline uint64_t
fix64_impl_add_u128(uint64_t x_hi, uint64_t x_lo, uint64_t y_hi, uint64_t y_lo, uint64_t *hi) {
    __extension__ unsigned __int128 res =
        (((unsigned __int128)x_hi << 64) | x_lo) + (((unsigned __int128)y_hi << 64) | y_lo);
    *hi = res >> 64;
    return res;
}
static inline uint64_t
fix64_impl_add_i128(int64_t x_hi, uint64_t x_lo, int64_t y_hi, uint64_t y_lo, int64_t *hi) {
    __extension__ __int128 res = (((__int128)x_hi << 64) | x_lo) + (((__int128)y_hi << 64) | y_lo);
    *hi = res >> 64;
    return res;
}
static inline uint64_t
fix64_impl_sub_u128(uint64_t x_hi, uint64_t x_lo, uint64_t y_hi, uint64_t y_lo, uint64_t *hi) {
    __extension__ unsigned __int128 res =
        (((unsigned __int128)x_hi << 64) | x_lo) - (((unsigned __int128)y_hi << 64) | y_lo);
    *hi = res >> 64;
    return res;
}
static inline uint64_t
fix64_impl_sub_i128(int64_t x_hi, uint64_t x_lo, int64_t y_hi, uint64_t y_lo, int64_t *hi) {
    __extension__ __int128 res = (((__int128)x_hi << 64) | x_lo) - (((__int128)y_hi << 64) | y_lo);
    *hi = res >> 64;
    return res;
}
static inline uint64_t fix64_impl_mul_u64_u128(uint64_t x, uint64_t y, uint64_t *hi) {
    __extension__ unsigned __int128 res = (unsigned __int128)x * y;
    *hi = res >> 64;
    return res;
}
static inline uint64_t fix64_impl_mul_i64_i128(int64_t x, int64_t y, int64_t *hi) {
    __extension__ __int128 res = (__int128)x * y;
    *hi = res >> 64;
    return res;
}
static inline uint64_t fix64_impl_mul_i64_u64_i128(int64_t x, uint64_t y, int64_t *hi) {
    __extension__ __int128 res = (__int128)x * y;
    *hi = res >> 64;
    return res;
}
#else  // if !FIX64_IMPL_USE_INT128
static inline uint64_t
fix64_impl_add_u128(uint64_t x_hi, uint64_t x_lo, uint64_t y_hi, uint64_t y_lo, uint64_t *hi) {
    uint64_t lo;
    int overflow = fix64_impl_add_u64_overflow(x_lo, y_lo, &lo);
    *hi = x_hi + y_hi + overflow;
    return lo;
}
static inline uint64_t
fix64_impl_add_i128(int64_t x_hi, uint64_t x_lo, int64_t y_hi, uint64_t y_lo, int64_t *hi) {
    uint64_t lo;
    int overflow = fix64_impl_add_u64_overflow(x_lo, y_lo, &lo);
    *hi = x_hi + y_hi + overflow;
    return lo;
}
static inline uint64_t
fix64_impl_sub_u128(uint64_t x_hi, uint64_t x_lo, uint64_t y_hi, uint64_t y_lo, uint64_t *hi) {
    uint64_t lo;
    int underflow = fix64_impl_sub_u64_underflow(x_lo, y_lo, &lo);
    *hi = x_hi - y_hi - underflow;
    return lo;
}
static inline uint64_t
fix64_impl_sub_i128(int64_t x_hi, uint64_t x_lo, int64_t y_hi, uint64_t y_lo, int64_t *hi) {
    uint64_t lo;
    int underflow = fix64_impl_sub_u64_underflow(x_lo, y_lo, &lo);
    *hi = x_hi - y_hi - underflow;
    return lo;
}
static inline uint64_t fix64_impl_mul_u64_u128(uint64_t x, uint64_t y, uint64_t *hi) {
    uint64_t xy_hi = (x >> 32) * (y >> 32);
    uint64_t xy_md = (x >> 32) * (uint64_t)(uint32_t)y;
    uint64_t yx_md = (y >> 32) * (uint64_t)(uint32_t)x;
    uint64_t xy_lo = (uint64_t)(uint32_t)x * (uint64_t)(uint32_t)y;

    uint64_t lo;
    int carry0 = fix64_impl_add_u64_overflow(xy_md << 32, yx_md << 32, &lo);
    int carry1 = fix64_impl_add_u64_overflow(lo, xy_lo, &lo);
    *hi = xy_hi + (xy_md >> 32) + (yx_md >> 32) + carry0 + carry1;
    return lo;
}
static inline uint64_t fix64_impl_mul_i64_i128(int64_t x, int64_t y, int64_t *hi) {
    uint64_t uhi;
    uint64_t lo = fix64_impl_mul_u64_u128((uint64_t)x, (uint64_t)y, &uhi);
    lo = fix64_impl_sub_u128(uhi, lo, (x < 0) ? y : 0, 0, &uhi);
    lo = fix64_impl_sub_u128(uhi, lo, (y < 0) ? x : 0, 0, &uhi);
    // Avoid UB of unsigned -> signed conversion, gets optimised out on both GCC and Clang
    *hi = (uhi > INT64_MAX) ? (int64_t)(uhi - INT64_MIN) + INT64_MIN : (int64_t)uhi;
    return lo;
}
static inline uint64_t fix64_impl_mul_i64_u64_i128(int64_t x, uint64_t y, int64_t *hi) {
    uint64_t uhi;
    uint64_t lo = fix64_impl_mul_u64_u128((uint64_t)x, y, &uhi);
    lo = fix64_impl_sub_u128(uhi, lo, (x < 0) ? y : 0, 0, &uhi);
    // Avoid UB of unsigned -> signed conversion, gets optimised out on both GCC and Clang
    *hi = (uhi > INT64_MAX) ? (int64_t)(uhi - INT64_MIN) + INT64_MIN : (int64_t)uhi;
    return lo;
}
#endif // if FIX64_IMPL_USE_INT128

// Don't try to implement division with __int128, it is at best as fast as our C/ASM combo fallback
// implementation, but can be slower
#if FIX64_IMPL_USE_NATIVE_DIVQ
static inline uint64_t
fix64_impl_div_u128_u64(uint64_t u_hi, uint64_t u_lo, uint64_t v) {
    if (FIX64_UNLIKELY(u_hi >= v)) {
        u_hi %= v;
    }

    uint64_t q, r;
    // clang-format off
    __asm__ (
        "divq\t%[v]"
        : "=a"(q), "=d"(r) // quotient ends up in rax, remainder in rdx
        : [v]"r"(v), "d"(u_hi), "a"(u_lo) // dividend goes in rdx:rax
        : "flags" // divq leaves flags in an undefined state
    );
    // clang-format on
    return q;
}

static inline int64_t fix64_impl_div_i128_i64(int64_t u_hi, uint64_t u_lo, int64_t v) {
    // unsigned variables
    uint64_t uu_hi = u_hi;
    uint64_t uu_lo = u_lo;
    uint64_t uv = v;

    // u_sign = -(u < 0)
    uint64_t u_sign = u_hi >> 63;

    // uu_hi:uu_lo = abs(u_hi:u_lo)
    uu_lo = fix64_impl_add_u128(uu_hi, uu_lo, u_sign, u_sign, &uu_hi);
    uu_lo ^= u_sign;
    uu_hi ^= u_sign;

    uint64_t v_sign = v >> 63; // = -(v < 0)
    uv = (v_sign) ? -uv : uv;  // = labs(v);

    uint64_t q_sign = u_sign ^ v_sign; // sign of the quotient

    uint64_t result = fix64_impl_div_u128_u64(uu_hi, uu_lo, uv);

    // branchless if (q_sign) negate result
    result ^= q_sign;
    result -= q_sign;

    return result;
}
#else
uint64_t fix64_impl_div_u128_u64(uint64_t u_hi, uint64_t u_lo, uint64_t v);
int64_t fix64_impl_div_i128_i64(int64_t u_hi, uint64_t u_lo, int64_t v);
#endif

static inline uint64_t fix64_impl_div_u128_u64_sat(uint64_t u_hi, uint64_t u_lo, uint64_t v) {
    if (FIX64_UNLIKELY(u_hi >= v)) {
        return UINT64_MAX;
    }
    return fix64_impl_div_u128_u64(u_hi, u_lo, v);
}

static inline int64_t fix64_impl_div_i128_i64_sat(int64_t u_hi, uint64_t u_lo, int64_t v) {
    // unsigned variables
    uint64_t uu_hi = u_hi;
    uint64_t uu_lo = u_lo;
    uint64_t uv = v;

    // u_sign = -(u < 0)
    uint64_t u_sign = u_hi >> 63;

    // uu_hi:uu_lo = abs(u_hi:u_lo)
    uu_lo = fix64_impl_add_u128(uu_hi, uu_lo, u_sign, u_sign, &uu_hi);
    uu_lo ^= u_sign;
    uu_hi ^= u_sign;

    uint64_t v_sign = v >> 63; // = -(v < 0)
    uv = (v_sign) ? -uv : uv;  // = labs(v);

    uint64_t q_sign = u_sign ^ v_sign; // sign of the quotient

    // handle INT64_MIN as a special case, if u_hi == INT64_MIN (i.e. MSB is set) division will
    // always overflow
    if (FIX64_UNLIKELY(uu_hi >> 63)) {
        return (q_sign) ? INT64_MIN : INT64_MAX;
    }

    uint64_t uu_lsh63 = (uu_hi << 1) | (uu_lo >> 63);
    // if v == INT64_MIN (i.e. MSB is set) division will never overflow if (u_hi != INT64_MIN)
    if (!FIX64_UNLIKELY(uv >> 63)) {
        if (q_sign) {
            // u positive, v negative => q negative
            // u_max = -0x8000_0000 * v + (-v - 1) => uu < 0x8000_0001 * uv
            // or u negative, v positive => q negative
            // u_min = -0x8000_0000 * v - (v - 1) => uu < 0x8000_0001 * uv
            if (FIX64_UNLIKELY(uu_lsh63 > uv || (uu_lsh63 == uv && (uu_lo << 1 >> 1) >= uv))) {
                return INT64_MIN;
            }
        } else {
            // u, v both positive => q positive
            // u_max = 0x7fff_ffff * v + (v - 1) => uu < 0x8000_0000 * uv
            // or u, v both negative => q positive
            // u_min = 0x7fff_ffff * v - (-v - 1) => uu < 0x8000_0000 * uv
            if (FIX64_UNLIKELY(uu_lsh63 >= uv)) {
                return INT64_MAX;
            }
        }
    }

    return fix64_impl_div_i128_i64(u_hi, u_lo, v);
}

#if FIX64_IMPL_USE_BUILTIN_CLZ
static inline unsigned fix64_impl_clz32(uint32_t arg) {
    // clz has UB when arg == 0, but this branch is optimised away pretty nicely on e.g. ARM where
    // the underlying clz instruction is defined
    return arg ? __builtin_clz(arg) : (CHAR_BIT * sizeof(arg));
}
static inline unsigned fix64_impl_clz64(uint64_t arg) {
    // clz has UB when arg == 0, but this branch is optimised away pretty nicely on e.g. ARM where
    // the underlying clz instruction is defined
    return arg ? __builtin_clzll(arg) : (CHAR_BIT * sizeof(arg));
}
#else
static inline unsigned fix64_impl_clz32(uint32_t arg) {
    unsigned result = 0;
    if (arg == 0) {
        return (CHAR_BIT * sizeof(arg));
    }
    for (unsigned sh = (CHAR_BIT * sizeof(arg)) >> 1; sh; sh >>= 1) {
        uint32_t tmp = arg >> sh;
        if (tmp) {
            arg = tmp;
        } else {
            result |= sh;
        }
    }
    return result;
}
static inline unsigned fix64_impl_clz64(uint64_t arg) {
    unsigned result = 0;
    if (arg == 0) {
        return (CHAR_BIT * sizeof(arg));
    }
    for (unsigned sh = (CHAR_BIT * sizeof(arg)) >> 1; sh; sh >>= 1) {
        uint32_t tmp = arg >> sh;
        if (tmp) {
            arg = tmp;
        } else {
            result |= sh;
        }
    }
    return result;
}
#endif
