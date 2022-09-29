#include <stdio.h>
#include <math.h>

#include <fix64.h>

#include "common.h"

#ifndef M_PI
    #define M_PI 3.141592653589793238462643383279502884
#endif

int main() {
    double start = -2.0 * M_PI;
    double stop = 2.0 * M_PI;
    double step = range_step(start, stop, 1e7);

    for (double fltarg = start; fltarg <= stop; fltarg += step) {
        if ((fabs(fltarg + 1.5 * M_PI) < 1e-9) ||
            (fabs(fltarg + 0.5 * M_PI) < 1e-9) ||
            (fabs(fltarg - 0.5 * M_PI) < 1e-9) ||
            (fabs(fltarg - 1.5 * M_PI) < 1e-9)) {
            // Error increases near discontinuities
            continue;
        }

        fix64_t arg = fix64_from_dbl(fltarg);

        fix64_t expected = fix64_from_dbl(tan(fix64_to_dbl(arg)));
        fix64_t result = fix64_tan(arg);

        if (!approx_eq(result, expected)) {
            printf("tan(%.10f [0x%016lx])\n", fix64_to_dbl(arg), arg.repr);
            printf("expected %.10f [0x%016lx]\n", fix64_to_dbl(expected), expected.repr);
            printf("got      %.10f [0x%016lx]\n", fix64_to_dbl(result), result.repr);
            return 1;
        }
    }

    return 0;
}
