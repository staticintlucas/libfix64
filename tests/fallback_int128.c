#include <bits/stdint-intn.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include <fix64.h>

#define INCR ((int)1e9 - 1)

int main() {
    for (uint64_t x = 0; x <= (UINT32_MAX - INCR); x += INCR) {
        for (uint64_t y = 0; y <= (UINT32_MAX - INCR); y += INCR) {
            uint64_t exp_hi = (x + y) >> 32;
            uint64_t exp_lo = (x + y) << 32;
            uint64_t hi;
            uint64_t lo = fix64_impl_add_u128(x >> 32, x << 32, y >> 32, y << 32, &hi);

            if (hi != exp_hi || lo != exp_lo) {
                printf("add_u128(0x%08" PRIx32 "%016" PRIx64 "00000000, 0x%08" PRIx32 "%016" PRIx64 "00000000) -> 0x%016" PRIx64 "%016" PRIx64 "; expected 0x%016" PRIx64 "%016" PRIx64 "\n", UINT32_C(0), x, UINT32_C(0), y, hi, lo, exp_hi, exp_lo);
                return 1;
            }
        }
    }

    for (int64_t x = INT32_MIN; x <= (INT32_MAX - INCR); x += INCR) {
        for (int64_t y = INT32_MIN; y <= (INT32_MAX - INCR); y += INCR) {
            int64_t exp_hi = (x + y) >> 32;
            uint64_t exp_lo = (uint64_t)(x + y) << 32;
            int64_t hi;
            uint64_t lo = fix64_impl_add_i128(x >> 32, x << 32, y >> 32, y << 32, &hi);

            if (hi != exp_hi || lo != exp_lo) {
                printf("add_u128(0x%08" PRIx32 "%016" PRIx64 "00000000, 0x%08" PRIx32 "%016" PRIx64 "00000000) -> 0x%016" PRIx64 "%016" PRIx64 "; expected 0x%016" PRIx64 "%016" PRIx64 "\n", (uint32_t)(x >> 63), x, (uint32_t)(y >> 63), y, hi, lo, exp_hi, exp_lo);
                return 1;
            }
        }
    }
    for (uint64_t x = 0; x <= (UINT32_MAX - INCR); x += INCR) {
        for (uint64_t y = 0; y <= (UINT32_MAX - INCR); y += INCR) {
            uint64_t exp_hi = (int64_t)(x - y) >> 32; // cast for arithmetic shift
            uint64_t exp_lo = (x - y) << 32;
            uint64_t hi;
            uint64_t lo = fix64_impl_sub_u128(x >> 32, x << 32, y >> 32, y << 32, &hi);

            if (hi != exp_hi || lo != exp_lo) {
                printf("sub_u128(0x%08" PRIx32 "%016" PRIx64 "00000000, 0x%08" PRIx32 "%016" PRIx64 "00000000) -> 0x%016" PRIx64 "%016" PRIx64 "; expected 0x%016" PRIx64 "%016" PRIx64 "\n", UINT32_C(0), x, UINT32_C(0), y, hi, lo, exp_hi, exp_lo);
                return 1;
            }
        }
    }

    for (int64_t x = INT32_MIN; x <= (INT32_MAX - INCR); x += INCR) {
        for (int64_t y = INT32_MIN; y <= (INT32_MAX - INCR); y += INCR) {
            int64_t exp_hi = (x - y) >> 32;
            uint64_t exp_lo = (uint64_t)(x - y) << 32;
            int64_t hi;
            uint64_t lo = fix64_impl_sub_i128(x >> 32, x << 32, y >> 32, y << 32, &hi);

            if (hi != exp_hi || lo != exp_lo) {
                printf("sub_u128(0x%08" PRIx32 "%016" PRIx64 "00000000, 0x%08" PRIx32 "%016" PRIx64 "00000000) -> 0x%016" PRIx64 "%016" PRIx64 "; expected 0x%016" PRIx64 "%016" PRIx64 "\n", (uint32_t)(x >> 63), x, (uint32_t)(y >> 63), y, hi, lo, exp_hi, exp_lo);
                return 1;
            }
        }
    }
}
