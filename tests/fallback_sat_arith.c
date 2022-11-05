#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include <fix64.h>

#define INCR ((int64_t)1e15)

int main() {
    for (uint64_t x = 0; x <= (UINT64_MAX - INCR); x += INCR) {
        for (uint64_t y = 0; y <= (UINT64_MAX - INCR); y += INCR) {
            uint64_t sum;
            int expected = UINT64_MAX - x < y;
            int result = fix64_impl_add_u64_overflow(x, y, &sum);

            if (result != expected) {
                printf("add_u64_overflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &sum) -> %d; expected %d\n", x, y, result, expected);
                return 1;
            }

            if (x + y != sum) {
                printf("add_u64_overflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &sum); sum = 0x%016" PRIx64 " expected 0x%016" PRIx64 "\n", x, y, x + y, sum);
                return 1;
            }
        }
    }

    for (int64_t x = INT64_MIN; x <= (INT64_MAX - INCR); x += INCR) {
        for (int64_t y = INT64_MIN; y <= (INT64_MAX - INCR); y += INCR) {
            int64_t sum;
            int expected = (x > 0) ? INT64_MAX - x < y : INT64_MIN - x > y;
            int result = fix64_impl_add_i64_overflow(x, y, &sum);

            if (result != expected) {
                printf("add_i64_overflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &sum) -> %d; expected %d\n", x, y, result, expected);
                return 1;
            }

            if (x + y != sum) {
                printf("add_i64_overflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &sum); sum = 0x%016" PRIx64 " expected 0x%016" PRIx64 "\n", x, y, x + y, sum);
                return 1;
            }
        }
    }

    for (uint64_t x = 0; x <= (UINT64_MAX - INCR); x += INCR) {
        for (uint64_t y = 0; y <= (UINT64_MAX - INCR); y += INCR) {
            uint64_t diff;
            int expected = x < y;
            int result = fix64_impl_sub_u64_underflow(x, y, &diff);

            if (result != expected) {
                printf("sub_u64_underflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &diff) -> %d; expected %d\n", x, y, result, expected);
                return 1;
            }

            if (x - y != diff) {
                printf("sub_u64_underflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &diff); diff = 0x%016" PRIx64 " expected 0x%016" PRIx64 "\n", x, y, x - y, diff);
                return 1;
            }
        }
    }

    for (int64_t x = INT64_MIN; x <= (INT64_MAX - INCR); x += INCR) {
        for (int64_t y = INT64_MIN; y <= (INT64_MAX - INCR); y += INCR) {
            int64_t diff;
            int expected = (x > 0) ? x - INT64_MAX > y : x - INT64_MIN < y;
            int result = fix64_impl_sub_i64_underflow(x, y, &diff);

            if (result != expected) {
                printf("sub_i64_underflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &diff) -> %d; expected %d\n", x, y, result, expected);
                return 1;
            }

            if (x - y != diff) {
                printf("sub_i64_underflow(0x%016" PRIx64 ", 0x%016" PRIx64 ", &diff); diff = 0x%016" PRIx64 " expected 0x%016" PRIx64 "\n", x, y, x - y, diff);
                return 1;
            }
        }
    }
}
