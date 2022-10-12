#include <stdio.h>
#include <math.h>
#include <string.h>

#include <fix64.h>

#include "common.h"

struct test {
    const char *exp;
    fix64_t val;
    fix64_fmt_param_t fmt;
};

typedef unsigned char uchar;

int main() {
    struct test tests[] = {
        {"0", FIX64_ZERO, { .base = FIX64_BASE_DECIMAL, .decimals = -100 }},
        {"0", FIX64_ZERO, { .base = FIX64_BASE_HEXADECIMAL, .decimals = -100 }},
        {"0", FIX64_ZERO, { .base = FIX64_BASE_OCTAL, .decimals = -100 }},
        {"0", FIX64_ZERO, { .base = FIX64_BASE_BINARY, .decimals = -100 }},
        {"0.00000", FIX64_ZERO, { .base = FIX64_BASE_DECIMAL, .decimals = 5 }},
        {"0.00000", FIX64_ZERO, { .base = FIX64_BASE_HEXADECIMAL, .decimals = 5 }},
        {"0.0000", FIX64_ZERO, { .base = FIX64_BASE_OCTAL, .decimals = 4 }},
        {"0.00000000", FIX64_ZERO, { .base = FIX64_BASE_BINARY, .decimals = 8 }},
        {"0", FIX64_ZERO, { .base = FIX64_BASE_DECIMAL, .decimals = -100, .base_pfx = 1 }},
        {"0x0", FIX64_ZERO, { .base = FIX64_BASE_HEXADECIMAL, .decimals = -100, .base_pfx = 1 }},
        {"0o0", FIX64_ZERO, { .base = FIX64_BASE_OCTAL, .decimals = -100, .base_pfx = 1 }},
        {"0b0", FIX64_ZERO, { .base = FIX64_BASE_BINARY, .decimals = -100, .base_pfx = 1 }},
        {"3.142", FIX64_PI, { .base = FIX64_BASE_DECIMAL, .decimals = -3 }},
        {"0.a2fa", FIX64_2_PI, { .base = FIX64_BASE_HEXADECIMAL, .decimals = -4 }},
        {"0.54271", FIX64_LN2, { .base = FIX64_BASE_OCTAL, .decimals = -6 }},
        {"0.00000000000000000000000000000001", FIX64_EPSILON, { .base = FIX64_BASE_BINARY, .decimals = -32 }},
        {"0.0000000000000000000000000000001", FIX64_EPSILON, { .base = FIX64_BASE_BINARY, .decimals = -31 }},
        {"0.2", FIX64_C(0.2), { .base = FIX64_BASE_DECIMAL, .decimals = -10 }},
        {"0.33333333", FIX64_C(0.2), { .base = FIX64_BASE_HEXADECIMAL, .decimals = -100 }},
        {"-0x0.9999999a", FIX64_C(-0.6), { .base = FIX64_BASE_HEXADECIMAL, .decimals = -100, .base_pfx = 1 }},
        {"-0X0.9999999A", FIX64_C(-0.6), { .base = FIX64_BASE_HEXADECIMAL, .decimals = -100, .base_pfx = 1, .uppercase = 1 }},
        {"        10", FIX64_C(10), { .base = FIX64_BASE_DECIMAL, .width = 10, .base_pfx = 1 }},
        {"       0xa", FIX64_C(10), { .base = FIX64_BASE_HEXADECIMAL, .width = 10, .base_pfx = 1 }},
        {"      0o12", FIX64_C(10), { .base = FIX64_BASE_OCTAL, .width = 10, .base_pfx = 1 }},
        {"    0b1010", FIX64_C(10), { .base = FIX64_BASE_BINARY, .width = 10, .base_pfx = 1 }},
        {"0000000010", FIX64_C(10), { .base = FIX64_BASE_DECIMAL, .width = 10, .pad_0 = 1, .base_pfx = 1 }},
        {"0x0000000a", FIX64_C(10), { .base = FIX64_BASE_HEXADECIMAL, .width = 10, .pad_0 = 1, .base_pfx = 1 }},
        {"0o00000012", FIX64_C(10), { .base = FIX64_BASE_OCTAL, .width = 10, .pad_0 = 1, .base_pfx = 1 }},
        {"0b00001010", FIX64_C(10), { .base = FIX64_BASE_BINARY, .width = 10, .pad_0 = 1, .base_pfx = 1 }},
        {"5", FIX64_C(+5), { .base = FIX64_BASE_DECIMAL }},
        {"-5", FIX64_C(-5), { .base = FIX64_BASE_DECIMAL }},
        {"+5", FIX64_C(+5), { .base = FIX64_BASE_DECIMAL, .plus_sign = 1 }},
        {"-5", FIX64_C(-5), { .base = FIX64_BASE_DECIMAL, .plus_sign = 1 }},
        {" 5", FIX64_C(+5), { .base = FIX64_BASE_DECIMAL, .space_sign = 1 }},
        {"-5", FIX64_C(-5), { .base = FIX64_BASE_DECIMAL, .space_sign = 1 }},
        {"+5", FIX64_C( 5), { .base = FIX64_BASE_DECIMAL, .space_sign = 1, .plus_sign = 1 }}, // plus_sign should override space_sign
        {"-5", FIX64_C(-5), { .base = FIX64_BASE_DECIMAL, .space_sign = 1, .plus_sign = 1 }},
        {"+0B01111111111111111111111111111111.11111111111111111111111111111111", FIX64_MAX, { .base = FIX64_BASE_BINARY, .decimals = -100, .width = 68, .pad_0 = 1, .base_pfx = 1, .plus_sign = 1, .uppercase = 1 }},
        {"-0b10000000000000000000000000000000.00000000000000000000000000000000", FIX64_MIN, { .base = FIX64_BASE_BINARY, .decimals = 100, .base_pfx = 1, .space_sign = 1 }},
    };
    const int len = 96;
    const int off = 16;
    char buf[len + 2 * off];
    char *ptr = buf + off;

    const size_t num_test = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < num_test; i++) {
        memset(buf, '~', sizeof(buf)); // Catch termination errors

        size_t result = fix64_to_str_fmt(ptr, tests[i].val, len, tests[i].fmt);

        if ((result != strlen(tests[i].exp)) || strcmp(ptr, tests[i].exp)) {
            printf("fix64_to_str_fmt(ptr, %f, sizeof(len), fmt) -> %zu; expected %zu\n",
                fix64_to_dbl(tests[i].val), result, strlen(tests[i].exp));
            printf("ptr -> %s; expected %s\n",
                ptr, tests[i].exp);
            printf("where fmt = {\n"
                "    .decimals   = %d,\n"
                "    .width      = %d,\n"
                "    .base       = %d,\n"
                "    .pad_0      = %d,\n"
                "    .base_pfx   = %d,\n"
                "    .plus_sign  = %d,\n"
                "    .space_sign = %d,\n"
                "    .uppercase  = %d,\n"
                "}",
                tests[i].fmt.decimals,
                tests[i].fmt.width,
                tests[i].fmt.base,
                tests[i].fmt.pad_0,
                tests[i].fmt.base_pfx,
                tests[i].fmt.plus_sign,
                tests[i].fmt.space_sign,
                tests[i].fmt.uppercase
            );
            return 1;
        }

        for (int j = 0; j < off; j++) {
            if (buf[j] != '~') {
                printf("buffer underflow! ptr[%d] -> %#02x; expected %#02x\n", j - off, (uchar)buf[j], (uchar)'~');
                printf("fix64_to_str_fmt(ptr, %f, sizeof(len), fmt)\n", fix64_to_dbl(tests[i].val));
                printf("where fmt = {\n"
                    "    .decimals   = %d,\n"
                    "    .width      = %d,\n"
                    "    .base       = %d,\n"
                    "    .pad_0      = %d,\n"
                    "    .base_pfx   = %d,\n"
                    "    .plus_sign  = %d,\n"
                    "    .space_sign = %d,\n"
                    "    .uppercase  = %d,\n"
                    "}",
                    tests[i].fmt.decimals,
                    tests[i].fmt.width,
                    tests[i].fmt.base,
                    tests[i].fmt.pad_0,
                    tests[i].fmt.base_pfx,
                    tests[i].fmt.plus_sign,
                    tests[i].fmt.space_sign,
                    tests[i].fmt.uppercase
                );
                return 1;
            }

            if (ptr[result + 1 + j] != '~') {
                printf("buffer overflow! ptr[result + 1 + %d] -> %#02x; expected %#02x\n", j, (uchar)ptr[result + 1 + j], (uchar)'~');
                printf("fix64_to_str_fmt(ptr, %f, sizeof(len), fmt)\n", fix64_to_dbl(tests[i].val));
                printf("where fmt = {\n"
                    "    .decimals   = %d,\n"
                    "    .width      = %d,\n"
                    "    .base       = %d,\n"
                    "    .pad_0      = %d,\n"
                    "    .base_pfx   = %d,\n"
                    "    .plus_sign  = %d,\n"
                    "    .space_sign = %d,\n"
                    "    .uppercase  = %d,\n"
                    "}",
                    tests[i].fmt.decimals,
                    tests[i].fmt.width,
                    tests[i].fmt.base,
                    tests[i].fmt.pad_0,
                    tests[i].fmt.base_pfx,
                    tests[i].fmt.plus_sign,
                    tests[i].fmt.space_sign,
                    tests[i].fmt.uppercase
                );
                return 1;
            }
        }
    }

    return 0;
}
