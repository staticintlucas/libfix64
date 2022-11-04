#include "fix64.h"
#include "fix64/impl.h"

#include <stdint.h>

#if !(defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)))

uint64_t fix64_impl_div_u128_u64(uint64_t u_hi, uint64_t u_lo, uint64_t v, uint64_t *r) {

    // This function is based on libdivide's libdivide_128_div_64_to_64
    // See: https://github.com/ridiculousfish/libdivide
    // https://ridiculousfish.com/blog/posts/labor-of-division-episode-v.html
    // https://ridiculousfish.com/blog/posts/labor-of-division-episode-vi.html

    // Check to make sure the result fits in a uint64_t
    // If not return UINT64_MAX for both quotient and remainder
    if (FIX64_UNLIKELY(u_hi >= v)) {
        *r = INT64_MAX;
        return INT64_MAX;
    }

    // normalise divisor and dividend
    uint32_t shift = fix64_impl_clz64(v); // shift for normalization

    // if (shift) performs better than the bit manip done by libdivide to avoid UB of u_lo >> 64
    // when shift == 0. Implementation is ~20% faster on AMD (Zen1) and ~5% on Intel (Skylake)
    if (shift) {
        // shift v by the number of leading 0's
        v <<= shift;
        // shift u by the same amount - this won't overflow since we know u_hi < v
        u_hi = (u_hi << shift) | (u_lo >> (64 - shift));
        u_lo <<= shift;
    }

    // lower digits of the normalised dividend
    uint32_t nu_hi = (uint32_t)(u_lo >> 32);
    uint32_t nu_lo = (uint32_t)u_lo;
    // normalised divisor
    uint32_t nv_hi = (uint32_t)(v >> 32);
    uint32_t nv_lo = (uint32_t)v;

    // estimate q_hi = u[127:32] / v[63:0] ~= u[127:64] / v[63:32]
    // q_hi <= q_est <= q_hi+2, which we correct for in the next step
    uint64_t q_est = u_hi / nv_hi;
    uint64_t r_est = u_hi % nv_hi;

    // estimate the correct remainder
    uint64_t r_pos = (r_est << 32) + nu_hi;
    uint64_t r_neg = q_est * nv_lo;
    uint64_t rem = r_pos - r_neg;

    // note: we use the 'sub branchless' version from ridiculousfish's blog. On Intel and Apple
    // M1 this is marginally slower than 'mul' but significantly faster than 'sub', while on AMD
    // this is marginally slower than 'sub' but significantly faster than 'mul'. For details
    // see: https://ridiculousfish.com/blog/posts/labor-of-division-episode-vi.html

    // correct q_est and rem
    uint64_t dq_1 = -(rem > r_pos);    // r_neg > r_pos, but it's faster this way
    uint64_t dq_2 = dq_1 & (-rem > v); // r_neg - r_pos > v
    q_est += dq_1 - dq_2;
    rem += (v & dq_1) << dq_2;
    uint32_t q_hi = (uint32_t)q_est;

    // estimate q_lo = u[95:0] / v[63:0] ~= u[95:32] / v[63:32]
    // q_lo <= q_est <= q_lo+2, which we correct for in the next step
    q_est = rem / nv_hi;
    r_est = rem % nv_hi;

    // estimate the correct remainder
    r_pos = (r_est << 32) + nu_lo;
    r_neg = q_est * nv_lo;
    rem = r_pos - r_neg;

    // correct q_est and rem
    dq_1 = -(rem > r_pos);    // r_neg > r_pos, but it's faster this way
    dq_2 = dq_1 & (-rem > v); // r_neg - r_pos > v
    q_est += dq_1 - dq_2;
    rem += (v & dq_1) << dq_2;
    uint32_t q_lo = (uint32_t)q_est;

    *r = rem >> shift;
    return ((uint64_t)q_hi << 32) | q_lo;
}

int64_t fix64_impl_div_i128_i64(int64_t u_hi, uint64_t u_lo, int64_t v, int64_t *r) {
    // unsigned variables
    uint64_t uu_hi = u_hi;
    uint64_t uu_lo = u_lo;
    uint64_t uv = v;
    uint64_t ur;

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
        *r = (q_sign) ? INT64_MIN : INT64_MAX;
        return (q_sign) ? INT64_MIN : INT64_MAX;
    }

    uint64_t uu_lsh63 = (uu_hi << 1) | (uu_lo >> 63);

    // if v == INT64_MIN (i.e. MSB is set) division will never overflow if (u_hi != INT64_MIN)
    if (!FIX64_UNLIKELY(uv >> 63)) {
        if (q_sign) {
            // u, v both positive => q positive
            // u_max = 0x7fff_ffff * v + (v - 1) => uu < 0x8000_0000 * uv
            // or u, v both negative => q positive
            // u_min = 0x7fff_ffff * v - (-v - 1) => uu < 0x8000_0000 * uv
            if (FIX64_UNLIKELY(uu_lsh63 >= uv)) {
                *r = INT64_MAX;
                return INT64_MAX;
            }
        } else {
            // u positive, v negative => q negative
            // u_max = -0x8000_0000 * v + (-v - 1) => uu < 0x8000_0001 * uv
            // or u negative, v positive => q negative
            // u_min = -0x8000_0000 * v - (v - 1) => uu < 0x8000_0001 * uv
            if (FIX64_UNLIKELY(uu_lsh63 > uv || (uu_lsh63 == uv && (uu_lo << 1 >> 1) >= uv))) {
                *r = INT64_MIN;
                return INT64_MIN;
            }
        }
    }

    uint64_t result = fix64_impl_div_u128_u64(uu_hi, uu_lo, uv, &ur);

    // branchless if (q_sign) negate result & ur
    result += q_sign;
    ur += q_sign;
    result ^= q_sign;
    ur ^= q_sign;

    *r = ur;
    return result;
}

#endif
