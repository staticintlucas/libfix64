#include "fix64.h"
#include "fix64/impl.h"

#include <stdint.h>

#if !FIX64_IMPL_USE_NATIVE_DIVQ

uint64_t fix64_impl_div_u128_u64(uint64_t u_hi, uint64_t u_lo, uint64_t v) {

    // This function is based on libdivide's libdivide_128_div_64_to_64
    // See: https://github.com/ridiculousfish/libdivide
    // https://ridiculousfish.com/blog/posts/labor-of-division-episode-v.html
    // https://ridiculousfish.com/blog/posts/labor-of-division-episode-vi.html

    // Check to make sure the result fits in a uint64_t
    // If not use u_hi % v which will be equivalent to having the quotient wrap
    if (FIX64_UNLIKELY(u_hi >= v)) {
        u_hi %= v;
    }

    // normalise divisor and dividend
    uint32_t shift = fix64_impl_clz64(v); // shift for normalization

    // shift v by the number of leading 0's
    v <<= shift;
    // shift u by the same amount - this won't overflow since we know u_hi < v
    // Check shift != 0 to avoid UB of u_lo >> 64. On ARM (where this code mainly ends up running)
    // this is optimised out anyway since lsr's result is 0 for shifts > 64. This performs better
    // than the bit manip done by libdivide which certain compilers struggle to optimise out
    u_hi = (shift) ? ((u_hi << shift) | (u_lo >> (64 - shift))) : u_hi;
    u_lo <<= shift;

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
    uint64_t dq_1 = -(rem > r_pos); // r_neg > r_pos, but it's faster this way
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
    dq_1 = -(rem > r_pos); // r_neg > r_pos, but it's faster this way
    dq_2 = dq_1 & (-rem > v); // r_neg - r_pos > v
    q_est += dq_1 - dq_2;
    // rem += (v & dq_1) << dq_2;
    uint32_t q_lo = (uint32_t)q_est;

    // r = rem >> shift;
    return ((uint64_t)q_hi << 32) | q_lo;
}

int64_t fix64_impl_div_i128_i64(int64_t u_hi, uint64_t u_lo, int64_t v) {
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
    uv = (v_sign) ? -uv : uv; // = labs(v);

    uint64_t result = fix64_impl_div_u128_u64(uu_hi, uu_lo, uv);

    uint64_t q_sign = u_sign ^ v_sign; // sign of the quotient
    result = (q_sign) ? -result : result;

    return result;
}

#endif
