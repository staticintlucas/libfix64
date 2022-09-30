#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#include <fix64.h>

#include "common.h"

int main() {
    fix64_t zero = FIX64_C(0.);
    if (zero.repr != 0) {
        printf("FIX64_C(0.).repr = %#"PRIx64", expected 0\n", zero.repr);
        return 1;
    }

    fix64_t one = FIX64_C(1.);
    if (one.repr != (INT64_C(1) << FIX64_FRAC_BITS)) {
        printf("FIX64_C(1.).repr = %#"PRIx64", expected %#"PRIx64"\n", one.repr, (INT64_C(1) << FIX64_FRAC_BITS));
        return 1;
    }

    fix64_t max = FIX64_C(0x1p31);
    if (max.repr != INT64_MAX) {
        printf("FIX64_C(0x1p31).repr = %#"PRIx64", expected %#"PRIx64"\n", max.repr, INT64_MAX);
        return 1;
    }

    fix64_t min = FIX64_C(-0x1p31);
    if (min.repr != INT64_MIN) {
        printf("FIX64_C(-0x1p31).repr = %#"PRIx64", expected %#"PRIx64"\n", min.repr, INT64_MIN);
        return 1;
    }

    fix64_t forty_two = FIX64_C(42.);
    if (forty_two.repr != (INT64_C(42) << FIX64_FRAC_BITS)) {
        printf("FIX64_C(42.).repr = %#"PRIx64", expected %#"PRIx64"\n", forty_two.repr, (INT64_C(42) << FIX64_FRAC_BITS));
        return 1;
    }

    fix64_t minus_nine_and_a_half = FIX64_C(-9.5);
    if (minus_nine_and_a_half.repr != -19 * (INT64_C(1) << (FIX64_FRAC_BITS - 1))) {
        printf("FIX64_C(-9.5).repr = %#"PRIx64", expected %#"PRIx64"\n", minus_nine_and_a_half.repr, -19 * (INT64_C(1) << (FIX64_FRAC_BITS - 1)));
        return 1;
    }

    return 0;
}
