// This header defines several inline utility functions which should not be placed in fix64.h

#pragma once

#include <limits.h>
#include <stdint.h>

static inline unsigned clz32(uint32_t arg) {
#if __has_builtin(__builtin_clz)
    // clz has UB when arg == 0, but this branch is optimised away pretty nicely on e.g. ARM where
    // the underlying clz instruction is defined
    return arg ? __builtin_clz(arg) : (CHAR_BIT * sizeof(arg));
#else
    unsigned result = 0;
    if (arg == 0) {
        return (CHAR_BIT * sizeof(arg));
    }
    for (unsigned sh = (CHAR_BIT * sizeof(arg)) >> 1; sh; sh >>= 1) {
        uint32_t tmp = arg >> sh;
        if (tmp) {
            arg = tmp;
        } else {
            result |= sh;
        }
    }
    return result;
#endif
}

static inline unsigned clz64(uint64_t arg) {
#if __has_builtin(__builtin_clzll)
    // clz has UB when arg == 0, but this branch is optimised away pretty nicely on e.g. ARM where
    // the underlying clz instruction is defined
    return arg ? __builtin_clzll(arg) : (CHAR_BIT * sizeof(arg));
#else
    unsigned result = 0;
    if (arg == 0) {
        return (CHAR_BIT * sizeof(arg));
    }
    for (unsigned sh = (CHAR_BIT * sizeof(arg)) >> 1; sh; sh >>= 1) {
        uint32_t tmp = arg >> sh;
        if (tmp) {
            arg = tmp;
        } else {
            result |= sh;
        }
    }
    return result;
#endif
}
