#include "fix64/impl.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include <fix64.h>

#define INCR ((int)1e6 - 1)

int main() {
    for (unsigned shift = 0; shift <= 32; shift += 2) {
        for (uint64_t x = 0; x <= (UINT32_MAX - INCR); x += INCR) {
            for (uint64_t y = 1; y <= (UINT32_MAX - INCR); y += INCR) {
                uint64_t exp = x << shift;
                uint64_t v = y << shift;
                uint64_t u_hi;
                uint64_t u_lo = fix64_impl_mul_u64_u128(exp, v, &u_hi);

                uint64_t q = fix64_impl_div_u128_u64(u_hi, u_lo, v);

                if (q != exp) {
                    printf("div_u128_u64(0x%016" PRIx64 "%016" PRIx64 ", 0x%016" PRIx64 ", &rem) -> 0x%016" PRIx64 "; expected 0x%016" PRIx64 "\n", u_hi, u_lo, v, q, exp);
                    return 1;
                }
            }
        }
    }

    for (unsigned shift = 0; shift <= 32; shift += 2) {
        for (int64_t x = INT32_MIN; x <= (INT32_MAX - INCR); x += INCR) {
            for (int64_t y = INT32_MIN; y <= (INT32_MAX - INCR); y += INCR) {
                int64_t exp = x << shift;
                int64_t v = y << shift;
                int64_t u_hi;
                uint64_t u_lo = fix64_impl_mul_i64_i128(exp, v, &u_hi);

                int64_t q = fix64_impl_div_i128_i64(u_hi, u_lo, v);

                if (q != exp) {
                    printf("div_i128_i64(0x%016" PRIx64 "%016" PRIx64 ", 0x%016" PRIx64 ", &rem) -> 0x%016" PRIx64 "; expected 0x%016" PRIx64 "\n", u_hi, u_lo, v, q, exp);
                    return 1;
                }
            }
        }
    }
}
