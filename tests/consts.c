#include <stdio.h>
#include <math.h>

#include <fix64.h>

#include "common.h"

int main() {
    fix64_t zero = FIX64_C(0.);
    if (!approx_eq(FIX64_ZERO, zero)) {
        printf("FIX64_ZERO = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_ZERO), 0.);
        return 1;
    }

    fix64_t one = FIX64_C(1.);
    if (!approx_eq(FIX64_ONE, one)) {
        printf("FIX64_ONE = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_ONE), 1.);
        return 1;
    }

    fix64_t half = FIX64_C(0.5);
    if (!approx_eq(FIX64_HALF, half)) {
        printf("FIX64_HALF = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_HALF), 0.5);
        return 1;
    }

    fix64_t two = FIX64_C(2.);
    if (!approx_eq(FIX64_TWO, two)) {
        printf("FIX64_TWO = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_TWO), 2.);
        return 1;
    }

    fix64_t e = FIX64_C(2.718281828459);
    if (!approx_eq(FIX64_E, e)) {
        printf("FIX64_E = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_E), 2.718281828459);
        return 1;
    }

    fix64_t log2e = FIX64_C(1.442695040889);
    if (!approx_eq(FIX64_LOG2E, log2e)) {
        printf("FIX64_LOG2E = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_LOG2E), 1.442695040889);
        return 1;
    }

    fix64_t log10e = FIX64_C(0.434294481903);
    if (!approx_eq(FIX64_LOG10E, log10e)) {
        printf("FIX64_LOG10E = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_LOG10E), 0.434294481903);
        return 1;
    }

    fix64_t ln2 = FIX64_C(0.693147180560);
    if (!approx_eq(FIX64_LN2, ln2)) {
        printf("FIX64_LN2 = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_LN2), 0.693147180560);
        return 1;
    }

    fix64_t ln10 = FIX64_C(2.302585092994);
    if (!approx_eq(FIX64_LN10, ln10)) {
        printf("FIX64_LN10 = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_LN10), 2.302585092994);
        return 1;
    }

    fix64_t pi = FIX64_C(3.141592653590);
    if (!approx_eq(FIX64_PI, pi)) {
        printf("FIX64_PI = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_PI), 3.141592653590);
        return 1;
    }

    fix64_t pi_2 = FIX64_C(1.570796326795);
    if (!approx_eq(FIX64_PI_2, pi_2)) {
        printf("FIX64_PI_2 = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_PI_2), 1.570796326795);
        return 1;
    }

    fix64_t pi_4 = FIX64_C(0.785398163397);
    if (!approx_eq(FIX64_PI_4, pi_4)) {
        printf("FIX64_PI_4 = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_PI_4), 0.785398163397);
        return 1;
    }

    fix64_t c_1_pi = FIX64_C(0.318309886184);
    if (!approx_eq(FIX64_1_PI, c_1_pi)) {
        printf("FIX64_1_PI = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_1_PI), 0.318309886184);
        return 1;
    }

    fix64_t c_2_pi = FIX64_C(0.636619772368);
    if (!approx_eq(FIX64_2_PI, c_2_pi)) {
        printf("FIX64_2_PI = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_2_PI), 0.636619772368);
        return 1;
    }

    fix64_t c_2_sqrtpi = FIX64_C(1.128379167096);
    if (!approx_eq(FIX64_2_SQRTPI, c_2_sqrtpi)) {
        printf("FIX64_2_SQRTPI = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_2_SQRTPI), 1.128379167096);
        return 1;
    }

    fix64_t sqrt2 = FIX64_C(1.414213562373);
    if (!approx_eq(FIX64_SQRT2, sqrt2)) {
        printf("FIX64_SQRT2 = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_SQRT2), 1.414213562373);
        return 1;
    }

    fix64_t sqrt1_2 = FIX64_C(0.707106781187);
    if (!approx_eq(FIX64_SQRT1_2, sqrt1_2)) {
        printf("FIX64_SQRT1_2 = %.10f, expected %.10f\n", fix64_to_dbl(FIX64_SQRT1_2), 0.707106781187);
        return 1;
    }

    return 0;
}
