#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <fix64.h>

static inline int approx_eq(fix64_t x, fix64_t y) {
    int64_t rel_shr = 36;
    int64_t abs_tol = 1;

    int64_t diff = llabs(y.repr - x.repr);

    return ((diff <= (llabs(x.repr) >> rel_shr)) ||
        (diff <= (llabs(y.repr) >> rel_shr)) ||
        (diff <= abs_tol));
}

static inline double range_step(double start, double stop, double n_steps) {
    return (stop - start) / n_steps;
}
