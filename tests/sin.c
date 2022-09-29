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
        fix64_t arg = fix64_from_dbl(fltarg);

        fix64_t expected = fix64_from_dbl(sin(fix64_to_dbl(arg)));
        fix64_t result = fix64_sin(arg);

        if (!approx_eq(result, expected)) {
            printf("sin(%.10f) -> %.10f; expected %.10f\n",
                fix64_to_dbl(arg),
                fix64_to_dbl(result),
                fix64_to_dbl(expected));
            return 1;
        }
    }

    return 0;
}
