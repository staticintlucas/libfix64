#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include <fix64.h>

int main() {
    for (unsigned shift = 0; shift < 32; shift++) {
        uint32_t n1 = UINT32_C(1) << shift;
        uint32_t n2 = n1 - 1;

        int r1 = fix64_impl_clz32(n1);
        int r2 = fix64_impl_clz32(n2);

        int e1 = 32 - shift - 1;
        int e2 = 32 - shift;

        if (r1 != e1) {
            printf("clz32(0x%08" PRIx32 ") -> %d; expected %d\n", n1, r1, e1);
            return 1;
        }

        if (r2 != e2) {
            printf("clz32(0x%08" PRIx32 ") -> %d; expected %d\n", n2, r2, e2);
            return 1;
        }
    }

    for (unsigned shift = 0; shift < 64; shift++) {
        uint64_t n1 = UINT64_C(1) << shift;
        uint64_t n2 = n1 - 1;

        int r1 = fix64_impl_clz64(n1);
        int r2 = fix64_impl_clz64(n2);

        int e1 = 64 - shift - 1;
        int e2 = 64 - shift;

        if (r1 != e1) {
            printf("clz64(0x%016" PRIx64 ") -> %d; expected %d\n", n1, r1, e1);
            return 1;
        }

        if (r2 != e2) {
            printf("clz64(0x%016" PRIx64 ") -> %d; expected %d\n", n2, r2, e2);
            return 1;
        }
    }

    return 0;
}
