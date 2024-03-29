#include "fix64.h"
#include "fix64/impl.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "str.inc"

// Algorithm to calculate number of decimal digits based on https://commaok.xyz/post/lookup_tables/
// and https://lemire.me/blog/2021/06/03/computing-the-number-of-digits-of-an-integer-even-faster/
static unsigned digits(uint32_t arg) {
    // Use (arg | 1) because "0" still needs 1 digit
    return (arg + digits_coef_table[31 - fix64_impl_clz32(arg | 1)]) >> 32;
}

static inline const char *decimal_100(uint32_t x) {
    return &("00010203040506070809"
             "10111213141516171819"
             "20212223242526272829"
             "30313233343536373839"
             "40414243444546474849"
             "50515253545556575859"
             "60616263646566676869"
             "70717273747576777879"
             "80818283848586878889"
             "90919293949596979899"[x * 2]);
}

static char *fmt_frac_10(char *buf, uint64_t repr, unsigned prec) {
    const unsigned max_prec = sizeof(frac_10_rounding) / sizeof(frac_10_rounding[0]) - 1;
    prec = FIX64_UNLIKELY(prec > max_prec) ? max_prec : prec;

    uint32_t ipart = repr >> FIX64_FRAC_BITS;
    uint64_t fpart = (repr & ((UINT64_C(1) << FIX64_FRAC_BITS) - 1)) << (64 - FIX64_FRAC_BITS);

    // TODO add compiler-agnostic version? GCC/Clang currently produce pretty suboptimal signed
    // saturation code for any implementation that doesn't use __builtin_*_overflow
    if (__builtin_add_overflow(fpart, frac_10_rounding[prec], &fpart)) {
        // This won't overflow since ipart only uses 31 bits since it comes from a signed int32_t
        ipart++;
    }

    unsigned idigits = digits(ipart);
    char *end = buf + idigits;
    while (idigits > 1) {
        const char *d = decimal_100(ipart % 100);
        buf[--idigits] = d[1];
        buf[--idigits] = d[0];
        ipart /= 100;
    }
    if (idigits) {
        buf[--idigits] = '0' + ipart;
    }
    if (FIX64_UNLIKELY(!prec)) {
        *end = '\0';
        return end;
    }
    *(end++) = '.';

    while (prec >= 2) {
        prec -= 2;
        uint64_t tmp_hi;
        uint64_t tmp_lo = fix64_impl_mul_u64_u128(fpart, 100, &tmp_hi);
        const char *d = decimal_100(tmp_hi);
        *(end++) = *(d++);
        *(end++) = *d;
        fpart = tmp_lo;
    }
    if (prec) {
        uint64_t tmp_hi;
        fix64_impl_mul_u64_u128(fpart, 10, &tmp_hi);
        *(end++) = '0' + tmp_hi;
    }
    return end;
}

static char *fmt_frac_16(char *buf, uint64_t repr, unsigned prec, int upper) {
    const unsigned bits_per_digit = 4; // 4 bits per hex digit
    const unsigned digit_mask = (1 << bits_per_digit) - 1;
    const unsigned max_prec = FIX64_FRAC_BITS / bits_per_digit;
    prec = FIX64_UNLIKELY(prec > max_prec) ? max_prec : prec;

    const char *digits;
    if (upper) {
        digits = "0123456789ABCDEF";
    } else {
        digits = "0123456789abcdef";
    }

    // This won't overflow since ipart only uses 31 bits since it comes from a signed int32_t
    repr += FIX64_HALF.repr >> (prec * bits_per_digit);

    uint32_t ipart = repr >> FIX64_FRAC_BITS;
    uint64_t fpart = repr & ((UINT64_C(1) << FIX64_FRAC_BITS) - 1);

    // Use (arg | 1) because "0" still needs 1 digit
    unsigned idigits = (32 / bits_per_digit) - (fix64_impl_clz32(ipart | 1) / bits_per_digit);
    buf += idigits;
    char *end = buf;
    do {
        *(--buf) = digits[ipart & digit_mask];
        ipart >>= bits_per_digit;
    } while (ipart);

    if (FIX64_UNLIKELY(!prec)) {
        return end;
    }
    *(end++) = '.';

    while (prec) {
        prec--;
        fpart <<= bits_per_digit;
        *(end++) = digits[fpart >> FIX64_FRAC_BITS];
        fpart &= ((UINT64_C(1) << FIX64_FRAC_BITS) - 1);
    }
    return end;
}

static char *fmt_frac_8(char *buf, uint64_t repr, unsigned prec) {
    const unsigned bits_per_digit = 3; // 3 bits per octal digit
    const unsigned digit_mask = (1 << bits_per_digit) - 1;
    const unsigned max_prec = (FIX64_FRAC_BITS + (bits_per_digit - 1)) / bits_per_digit;
    prec = FIX64_UNLIKELY(prec > max_prec) ? max_prec : prec;

    // This won't overflow since ipart only uses 31 bits since it comes from a signed int32_t
    repr += FIX64_HALF.repr >> (prec * bits_per_digit);

    uint32_t ipart = repr >> FIX64_FRAC_BITS;
    uint64_t fpart = repr & ((UINT64_C(1) << FIX64_FRAC_BITS) - 1);

    // Use (arg | 1) because "0" still needs 1 digit
    unsigned idigits = ((32 - fix64_impl_clz32(ipart | 1)) + (bits_per_digit - 1)) / bits_per_digit;
    buf += idigits;
    char *end = buf;
    do {
        *(--buf) = '0' + (ipart & digit_mask);
        ipart >>= bits_per_digit;
    } while (ipart);

    if (FIX64_UNLIKELY(!prec)) {
        return end;
    }
    *(end++) = '.';

    while (prec) {
        prec--;
        fpart <<= bits_per_digit;
        *(end++) = '0' + (fpart >> FIX64_FRAC_BITS);
        fpart &= ((UINT64_C(1) << FIX64_FRAC_BITS) - 1);
    }
    return end;
}

static char *fmt_frac_2(char *buf, uint64_t repr, unsigned prec) {
    const unsigned max_prec = FIX64_FRAC_BITS;
    prec = FIX64_UNLIKELY(prec > max_prec) ? max_prec : prec;

    // This won't overflow since ipart only uses 31 bits since it comes from a signed int32_t
    repr += FIX64_HALF.repr >> prec;

    uint32_t ipart = repr >> FIX64_FRAC_BITS;
    uint64_t fpart = repr & ((UINT64_C(1) << FIX64_FRAC_BITS) - 1);

    // Use (arg | 1) because "0" still needs 1 digit
    unsigned idigits = 32 - fix64_impl_clz32(ipart | 1);
    buf += idigits;
    char *end = buf;
    do {
        *(--buf) = '0' + (ipart & 1);
        ipart >>= 1;
    } while (ipart);

    if (FIX64_UNLIKELY(!prec)) {
        return end;
    }
    *(end++) = '.';

    while (prec) {
        prec--;
        fpart <<= 1;
        *(end++) = '0' + (fpart >> FIX64_FRAC_BITS);
        fpart &= ((UINT64_C(1) << FIX64_FRAC_BITS) - 1);
    }
    return end;
}

size_t fix64_to_str_fmt(char *buf, fix64_t val, size_t size, fix64_fmt_param_t fmt) {

    char tmp_buf[256]; // number is formatted here, max width = 255 + 1 for the nul
    char *end = tmp_buf; // points to end of buf

    uint64_t repr = val.repr;
    int negative = 0;
    if (val.repr < 0) {
        repr = UINT64_C(0) - val.repr;
        negative = 1;
    }

    unsigned prec = fmt.decimals;
    int trim_0 = 0;
    if (fmt.decimals < 0) {
        prec = 0u - fmt.decimals;
        trim_0 = 1;
    }

    char num_buf[72]; // For binary formatting, 64 bits + '.' + alignment
    char *num_end; // points to end of num_buf
    if (fmt.base == FIX64_BASE_DECIMAL) {
        num_end = fmt_frac_10(num_buf, repr, prec);
    } else if (fmt.base == FIX64_BASE_HEXADECIMAL) {
        num_end = fmt_frac_16(num_buf, repr, prec, fmt.uppercase);
    } else if (fmt.base == FIX64_BASE_BINARY) {
        num_end = fmt_frac_2(num_buf, repr, prec);
    } else { // if (fmt.base == FIX64_BASE_OCTAL)
        num_end = fmt_frac_8(num_buf, repr, prec);
    }

    // Only trim zeros if we actually have a fractional part
    if (prec && trim_0) {
        while (num_end > num_buf && *(num_end - 1) == '0') {
            num_end--;
        }
        if (num_end > num_buf && *(num_end - 1) == '.') {
            num_end--;
        }
    }
    int64_t num_len = num_end - num_buf;

    int64_t n_pad = fmt.width - num_len;
    if (negative || fmt.plus_sign || fmt.space_sign) {
        n_pad--;
    }
    if (fmt.base_pfx && !(fmt.base == FIX64_BASE_DECIMAL)) {
        n_pad -= 2;
    }

    if (n_pad > 0 && !fmt.pad_0) { // i.e. pad with spaces
        memset(end, ' ', n_pad);
        end += n_pad;
    }

    if (negative) {
        *(end++) = '-';
    } else if (fmt.plus_sign) {
        *(end++) = '+';
    } else if (fmt.space_sign) {
        *(end++) = ' ';
    }

    if (fmt.base_pfx) {
        if (fmt.base == FIX64_BASE_BINARY) {
            *(end++) = '0';
            *(end++) = fmt.uppercase ? 'B' : 'b';
        } else if (fmt.base == FIX64_BASE_OCTAL) {
            *(end++) = '0';
            *(end++) = fmt.uppercase ? 'O' : 'o';
        } else if (fmt.base == FIX64_BASE_HEXADECIMAL) {
            *(end++) = '0';
            *(end++) = fmt.uppercase ? 'X' : 'x';
        }
    }

    if (n_pad > 0 && fmt.pad_0) {
        memset(end, '0', n_pad);
        end += n_pad;
    }

    memcpy(end, num_buf, num_len);
    end += num_len;
    *(end++) = '\0';

    size_t len = end - tmp_buf;
    if (len < size) {
        size = len;
    }
    if (buf && size) {
        memcpy(buf, tmp_buf, size);
    }
    return len - 1; // don't include the '\0' in the length
}
