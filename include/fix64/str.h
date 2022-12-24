#pragma once

#include <stddef.h>

#include "fix64.h"

//==========================================================
// String conversions
//==========================================================

/// Enum of the possible bases used for formatting fix64_t numbers
enum {
    FIX64_BASE_DECIMAL, ///< Format as a decimal number
    FIX64_BASE_BINARY, ///< Format as a binary number
    FIX64_BASE_OCTAL, ///< Format as an octal number
    FIX64_BASE_HEXADECIMAL, ///< Format as a hexadecimal number
};

/// Struct containing the possible parameters for converting fix64_t numbers to strings
typedef struct {
    /// The number of decimal digits to format. A negative value will format up to a maximum
    /// of -decimals digits while removing trailing zeros. A positive value will leave trailing
    /// zeros intact. A value of 0 will also remove the decimal point.
    int32_t decimals    : 8;
    /// The total width of the number. If the formatted number is smaller than this width, it will
    /// be padded either with spaces or with leading zeros, depending on the value of `pad_0`. The
    /// formatted number will not be truncated if it is larger than width. A value of 0 should be
    /// used when no padding is desired.
    uint32_t width      : 8;
    /// The base for formatting the number given by one of the `FIX64_BASE_*` constants
    uint32_t base       : 2;
    /// If set to 1, the number will be padded with leading zeros when padding is required as
    /// described in the `width` field. If set to 0 the number is padded with leading spaces.
    uint32_t pad_0      : 1;
    /// Whether to add a prefix for the given base. Adds a "0b" prefix for binary, "0o" for octal,
    /// or "0x" for hexadecimal. No prefix is added for decimal. If the `uppercase` field is set,
    /// the base is also in uppercase.
    uint32_t base_pfx   : 1;
    /// Whether to add a "+" prefix when formatting positive numbers
    uint32_t plus_sign  : 1;
    /// Whether to add a leading space when formatting positive numbers. If `plus_sign` is set this
    /// option is ignored.
    uint32_t space_sign : 1;
    /// Whether to format the number in uppercase. This affects extra digits for bases >=10 and
    /// letters in the base prefix if `base_pfx` is set.
    uint32_t uppercase  : 1;
} fix64_fmt_param_t;

/// Converts a fix64_t value to its string representation in the given format. If the buffer is too
/// short the result is truncated. If the destination buffer is NULL or the size is 0 no data is
/// written. The total number of characters that would have been written without truncation is
/// returned. Truncation can be detected by checking:
/// \code
///     if (fix64_to_str_fmt(buf, value, size, fmt) >= size) {
///         // truncation occurred!
///     }
/// \endcode
///
/// @param buf a string buffer to write the string to
/// @param value the fix64_t value to convert
/// @param size the total size of the string buffer used for bounds checking
/// @param fmt the format for the string
/// @return the number of characters that would have been written without truncation
size_t fix64_to_str_fmt(char *buf, fix64_t val, size_t size, fix64_fmt_param_t fmt);

/// Converts a fix64_t value to its string representation. If the buffer is too short the result is
/// truncated. If the destination buffer is NULL or the size is 0 no data is written. The total
/// number of characters that would have been written without truncation is returned. Truncation can
/// be detected by checking:
/// \code
///     if (fix64_to_str(buf, value, size) >= size) {
///         // truncation occurred!
///     }
/// \endcode
///
/// @param buf a string buffer to write the string to
/// @param value the fix64_t value to convert
/// @param size the total size of the string buffer used for bounds checking
/// @return the number of characters that would have been written without truncation
static inline size_t fix64_to_str(char *buf, fix64_t val, size_t size) {
    fix64_fmt_param_t fmt = { .decimals = 5 };
    return fix64_to_str_fmt(buf, val, size, fmt);
}

/// Converts a fix64_t value to its hexadecimal string representation. If the buffer is too short
/// the result is truncated. If the destination buffer is NULL or the size is 0 no data is written.
/// The total number of characters that would have been written without truncation is returned.
/// Truncation can be detected by checking:
/// \code
///     if (fix64_to_hex(buf, value, size) >= size) {
///         // truncation occurred!
///     }
/// \endcode
///
/// @param buf a string buffer to write the string to
/// @param value the fix64_t value to convert
/// @param size the total size of the string buffer used for bounds checking
/// @return the number of characters that would have been written without truncation
static inline size_t fix64_to_hex(char *buf, fix64_t val, size_t size) {
    fix64_fmt_param_t fmt = { .decimals = 4, .base = FIX64_BASE_HEXADECIMAL };
    return fix64_to_str_fmt(buf, val, size, fmt);
}
