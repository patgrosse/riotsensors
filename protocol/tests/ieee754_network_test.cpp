#include <gtest/gtest.h>
#include <inttypes.h>

#include <ieee754_network.h>

TEST(ieee754_network, single_precision) {
    float f = 3.1415926f, f2;
    uint32_t fi;

    fi = (uint32_t) pack754_32(f);
    f2 = (float) unpack754_32(fi);

    printf("float before : %.7f\n", f);
    printf("float encoded: 0x%08" PRIx32 "\n", fi);
    printf("float after  : %.7f\n", f2);

    ASSERT_FLOAT_EQ(f, f2);
}

TEST(ieee754_network, double_precision) {
    double d = 3.14159265358979323, d2;
    uint64_t di;

    di = pack754_64(d);
    d2 = (double) unpack754_64(di);

    printf("double before : %.20lf\n", d);
    printf("double encoded: 0x%016" PRIx64 "\n", di);
    printf("double after  : %.20lf\n", d2);

    ASSERT_DOUBLE_EQ(d, d2);
}