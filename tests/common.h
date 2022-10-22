#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <fix64.h>

static inline int approx_eq(fix64_t x, fix64_t y) {
    uint64_t rel_shr = 40;
    uint64_t abs_tol = 1;

    uint64_t diff = (uint64_t)llabs(y.repr - x.repr);
    uint64_t absx = (uint64_t)llabs(x.repr);
    uint64_t absy = (uint64_t)llabs(y.repr);

    return ((diff <= (absx >> rel_shr)) ||
        (diff <= (absy >> rel_shr)) ||
        (diff <= abs_tol));
}

static inline double range_step(double start, double stop, double n_steps) {
    return (stop - start) / n_steps;
}
