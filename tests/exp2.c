#include <stdio.h>
#include <math.h>

#include <fix64.h>

#include "common.h"

int main() {
    double start = -30.0;
    double stop = 30.0;
    double step = range_step(start, stop, 1e7);

    for (double fltarg = start; fltarg <= stop; fltarg += step) {
        fix64_t arg = fix64_from_dbl(fltarg);

        fix64_t expected = fix64_from_dbl(exp2(fix64_to_dbl(arg)));
        fix64_t result = fix64_exp2(arg);

        if (!approx_eq(result, expected)) {
            printf("exp2(%.10f) -> %.10f; expected %.10f\n",
                fix64_to_dbl(arg),
                fix64_to_dbl(result),
                fix64_to_dbl(expected));
            return 1;
        }
    }

    return 0;
}
