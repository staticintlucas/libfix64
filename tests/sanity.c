#include <float.h>
#include <stdio.h>

#include <fix64.h>

#include "common.h"

int main() {
    if (sizeof(int128_t) != 16) {
        printf("sizeof(int128_t) = %zu, expected 16\n", sizeof(int128_t));
        return 1;
    }

    if (sizeof(uint128_t) != 16) {
        printf("sizeof(uint128_t) = %zu, expected 16\n", sizeof(uint128_t));
        return 1;
    }

    if (FLT_RADIX != 2) {
        printf("FLT_RADIX = %u, expected 2\n", FLT_RADIX);
        return 1;
    }

    if (sizeof(fix64_t) != 8) {
        printf("sizeof(fix64_t) = %zu, expected 8\n", sizeof(fix64_t));
        return 1;
    }

    if (FIX64_FRAC_BITS != 32) {
        printf("FIX64_FRAC_BITS = %u, expected 32\n", FIX64_FRAC_BITS);
        return 1;
    }

    if (FIX64_INT_BITS != 31) {
        printf("FIX64_INT_BITS = %u, expected 31\n", FIX64_INT_BITS);
        return 1;
    }

    if (FIX64_BITS != 63) {
        printf("FIX64_BITS = %u, expected 63\n", FIX64_BITS);
        return 1;
    }

    // will be off by FIX64_EPSILON, but that's within our tolerance
    fix64_t exp_max = FIX64_C(0x1p31);
    if (!approx_eq(FIX64_MAX, exp_max)) {
        printf("FIX64_MAX = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_MAX), 0x1p31);
        return 1;
    }

    fix64_t exp_min = FIX64_C(-0x1p31);
    if (!approx_eq(FIX64_MIN, exp_min)) {
        printf("FIX64_MIN = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_MIN), -0x1p31);
        return 1;
    }

    fix64_t exp_eps = FIX64_C(0x1p-32);
    if (!approx_eq(FIX64_EPSILON, exp_eps)) {
        printf("FIX64_EPSILON = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_EPSILON), 0x1p-32);
        return 1;
    }

    return 0;
}
