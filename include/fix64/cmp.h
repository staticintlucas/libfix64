#pragma once

#include "fix64.h"

/// Compares two fix64_t values for equality
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return 1 if the values are equal, 0 otherwise
static inline int fix64_eq(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr == rhs.repr);
}

/// Compares two fix64_t values for inequality
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return 1 if the values are not equal, 0 otherwise
static inline int fix64_neq(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr != rhs.repr);
}

/// Check if a fix64_t is less than another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return 1 if lhs is less than rhs, 0 otherwise
static inline int fix64_lt(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr < rhs.repr);
}

/// Check if a fix64_t is greater than another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return 1 if lhs is greater than rhs, 0 otherwise
static inline int fix64_gt(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr > rhs.repr);
}

/// Check if a fix64_t is less than or equal to another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return 1 if lhs is less than or equal to rhs, 0 otherwise
static inline int fix64_lte(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr <= rhs.repr);
}

/// Check if a fix64_t is greater than or equal to another
///
/// @param lhs left hand side for comparison
/// @param rhs right hand side for comparison
/// @return 1 if lhs is greater than or equal to rhs, 0 otherwise
static inline int fix64_gte(fix64_t lhs, fix64_t rhs) {
    return (lhs.repr >= rhs.repr);
}
