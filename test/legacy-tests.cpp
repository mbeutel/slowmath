
    // These are old tests that need to be modernized and aligned with the style of the other test cases.

#include <array>
#include <tuple>
#include <limits>
#include <climits> // for CHAR_BIT
#include <cstdint>

#include <stdint.h>

#include <catch2/catch_test_macros.hpp>

#include <slowmath/arithmetic.hpp>

#include <gsl-lite/gsl-lite.hpp> // for fail_fast


using std::uint8_t;
using std::uint16_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int64_t;
using std::size_t;


const volatile uint8_t u8 = std::numeric_limits<uint8_t>::max() - 1;
const volatile uint64_t u64 = std::numeric_limits<uint64_t>::max() - 1;
const volatile int8_t i8n = std::numeric_limits<int8_t>::min() + 1;
const volatile int8_t i8nm = std::numeric_limits<int8_t>::min();
const volatile int64_t i64n = std::numeric_limits<int64_t>::min() + 1;
const volatile int8_t i8p = std::numeric_limits<int8_t>::max() - 1;
const volatile int64_t i64p = std::numeric_limits<int64_t>::max() - 1;
const volatile uint8_t zu8 = 0;
const volatile uint64_t zu64 = 0;
const volatile int8_t zi8 = 0;

const volatile size_t smax = std::numeric_limits<size_t>::max();
const volatile size_t smaxlog = slowmath::log_floori<size_t>(smax, size_t(2));
const volatile int imin = std::numeric_limits<int>::min(),
                   imax = std::numeric_limits<int>::max();
const volatile int imaxlog = slowmath::log_floori<int>(imax, 2);
const volatile int iminlog = slowmath::log_floori<int>(imin / -2, 2) + 1;
const volatile int64_t lmin = std::numeric_limits<int64_t>::min(),
                       lmax = std::numeric_limits<int64_t>::max();
const volatile int64_t lmaxlog = slowmath::log_floori<int64_t>(lmax, int64_t(2));
const volatile int64_t lminlog = slowmath::log_floori<int64_t>(lmin / -2, int64_t(2)) + 1;


TEST_CASE("arithmetic")
{
    volatile uint8_t lu8;
    volatile uint64_t lu64;
    volatile int8_t li8;
    volatile int64_t li64;

    SECTION("add")
    {
        CHECK_THROWS_AS(lu8 = slowmath::add_checked(u8, u8), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::add_checked(i8p, i8p), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::add_checked(i8n, i8n), std::system_error);
        CHECK_THROWS_AS(lu64 = slowmath::add_checked(u64, u64), std::system_error);
        CHECK_THROWS_AS(li64 = slowmath::add_checked(i64p, i64p), std::system_error);
        CHECK_THROWS_AS(li64 = slowmath::add_checked(i64n, i64n), std::system_error);
    }

    SECTION("subtract")
    {
        CHECK_THROWS_AS(lu8 = slowmath::subtract_checked(zu8, u8), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::subtract_checked(i8p, i8n), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::subtract_checked(i8n, i8p), std::system_error);
        CHECK_THROWS_AS(lu64 = slowmath::subtract_checked(zu64, u64), std::system_error);
        CHECK_THROWS_AS(li64 = slowmath::subtract_checked(i64p, i64n), std::system_error);
        CHECK_THROWS_AS(li64 = slowmath::subtract_checked(i64n, i64p), std::system_error);
    }

    SECTION("multiply")
    {
        CHECK_THROWS_AS(lu8 = slowmath::multiply_checked(u8, u8), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::multiply_checked(i8p, i8p), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::multiply_checked(i8p, i8n), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::multiply_checked(i8n, i8n), std::system_error);
        CHECK_THROWS_AS(lu64 = slowmath::multiply_checked(u64, u64), std::system_error);
        CHECK_THROWS_AS(li64 = slowmath::multiply_checked(i64p, i64p), std::system_error);
        CHECK_THROWS_AS(li64 = slowmath::multiply_checked(i64p, i64n), std::system_error);
        CHECK_THROWS_AS(li64 = slowmath::multiply_checked(i64n, i64n), std::system_error);
    }

    SECTION("divide")
    {
        CHECK_THROWS_AS(lu8 = slowmath::divide_checked(u8, zu8), gsl::fail_fast);
        CHECK_THROWS_AS(li8 = slowmath::divide_checked(i8p, zi8), gsl::fail_fast);
        CHECK_THROWS_AS(li8 = slowmath::divide_checked(i8nm, int8_t(-1)), std::system_error);
    }

    SECTION("modulo")
    {
        CHECK_THROWS_AS(lu8 = slowmath::modulo_checked(u8, zu8), gsl::fail_fast);
        CHECK_THROWS_AS(li8 = slowmath::modulo_checked(i8p, zi8), gsl::fail_fast);
        CHECK_THROWS_AS(li8 = slowmath::modulo_checked(i8nm, int8_t(-1)), std::system_error);
    }

    SECTION("shift_left")
    {
        CHECK_THROWS_AS(lu8 = slowmath::shift_left_checked(u8, uint8_t(10)), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::shift_left_checked(zi8, int8_t(10)), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::shift_left_checked(zi8, int8_t(-1)), gsl::fail_fast);
        CHECK_THROWS_AS(li8 = slowmath::shift_left_checked(i8n, int8_t(0)), gsl::fail_fast);
        CHECK_THROWS_AS(li8 = slowmath::shift_left_checked(i8p, int8_t(2)), std::system_error);
    }

    SECTION("shift_right")
    {
        CHECK_THROWS_AS(lu8 = slowmath::shift_right_checked(u8, uint8_t(10)), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::shift_right_checked(zi8, int8_t(10)), std::system_error);
        CHECK_THROWS_AS(li8 = slowmath::shift_right_checked(zi8, int8_t(-1)), gsl::fail_fast);
        CHECK_THROWS_AS(li8 = slowmath::shift_right_checked(i8n, int8_t(0)), gsl::fail_fast);
    }

    SECTION("multiply-extended")
    {
        SECTION("good")
        {
            CHECK(slowmath::multiply_checked(size_t(0), size_t(1)) == size_t(0));
            CHECK(slowmath::multiply_checked(size_t(2), size_t(3)) == size_t(6));
            CHECK(slowmath::multiply_checked(size_t(3), size_t(4)) == size_t(12));

            CHECK(slowmath::multiply_checked(smax, size_t(1)) == smax);
            CHECK(slowmath::multiply_checked(smax, size_t(0)) == size_t(0));
            CHECK(slowmath::multiply_checked(size_t(1), smax) == smax);
            CHECK(slowmath::multiply_checked(size_t(0), smax) == size_t(0));
            CHECK(slowmath::multiply_checked(smax / 2, size_t(2)) == (smax / size_t(2)) * size_t(2));
            CHECK(slowmath::multiply_checked(size_t(2), smax / 2) == (smax / size_t(2)) * size_t(2));

            CHECK(slowmath::multiply_checked(smax / 2 - 1, size_t(2)) == (smax / 2 - 1) * 2); // make sure the implementation doesn't use floating-point numbers

            CHECK(slowmath::multiply_checked(0, 1) == 0);
            CHECK(slowmath::multiply_checked(2, 3) == 6);
            CHECK(slowmath::multiply_checked(3, 4) == 12);

            CHECK(slowmath::multiply_checked(imax, 1) == imax);
            CHECK(slowmath::multiply_checked(imax, 0) == 0);
            CHECK(slowmath::multiply_checked(1, imax) == imax);
            CHECK(slowmath::multiply_checked(0, imax) == 0);
            CHECK(slowmath::multiply_checked(imax / 2, 2) == (imax / 2) * 2);
            CHECK(slowmath::multiply_checked(2, imax / 2) == (imax / 2) * 2);
            CHECK(slowmath::multiply_checked(imin / 2, 2) == imin);
            CHECK(slowmath::multiply_checked(2, imin / 2) == imin);
            CHECK(slowmath::multiply_checked(imin / -2, -2) == imin);
            CHECK(slowmath::multiply_checked(-2, imin / -2) == imin);

            CHECK(slowmath::multiply_checked(imin, 1) == imin);
            CHECK(slowmath::multiply_checked(1, imin) == imin);

            CHECK(slowmath::multiply_checked(imin + 1, -1) == imax);
            CHECK(slowmath::multiply_checked(-1, imin + 1) == imax);

            CHECK(slowmath::multiply_checked<int64_t>(lmax, 1) == lmax);
            CHECK(slowmath::multiply_checked<int64_t>(lmax, 0) == 0);
            CHECK(slowmath::multiply_checked<int64_t>(1, lmax) == lmax);
            CHECK(slowmath::multiply_checked<int64_t>(0, lmax) == 0);
            CHECK(slowmath::multiply_checked<int64_t>(lmax / 2, 2) == (lmax / 2) * 2);
            CHECK(slowmath::multiply_checked<int64_t>(2, lmax / 2) == (lmax / 2) * 2);
            CHECK(slowmath::multiply_checked<int64_t>(lmin / 2, 2) == lmin);
            CHECK(slowmath::multiply_checked<int64_t>(2, lmin / 2) == lmin);
            CHECK(slowmath::multiply_checked<int64_t>(lmin / -2, -2) == lmin);
            CHECK(slowmath::multiply_checked<int64_t>(-2, lmin / -2) == lmin);

            CHECK(slowmath::multiply_checked<int64_t>(lmax / 2 - 1, 2) == (lmax / 2 - 1) * 2); // make sure the implementation doesn't use floating-point numbers

            CHECK(slowmath::multiply_checked<int64_t>(lmin, 1) == lmin);
            CHECK(slowmath::multiply_checked<int64_t>(1, lmin) == lmin);

            CHECK(slowmath::multiply_checked<int64_t>(lmin + 1, -1) == lmax);
            CHECK(slowmath::multiply_checked<int64_t>(-1, lmin + 1) == lmax);

            CHECK(slowmath::multiply_checked(-1, 1) == -1);
            CHECK(slowmath::multiply_checked(1, -1) == -1);
        }

        SECTION("bad")
        {
            CHECK_THROWS_AS(slowmath::multiply_checked(smax, size_t(2)), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(size_t(2), smax), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(smax / size_t(2), size_t(4)), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(size_t(4), smax / size_t(2)), std::system_error);

            CHECK_THROWS_AS(slowmath::multiply_checked(imin, -1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(-1, imin), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(imin + 1, 2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(2, imin + 1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(imin + 1, -2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(-2, imin + 1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(imax - 1, -2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(-2, imax - 1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(imax - 1, 2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(2, imax - 1), std::system_error);

            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(lmin, -1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(-1, lmin), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(lmin + 1, 2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(2, lmin + 1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(lmin + 1, -2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(-2, lmin + 1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(lmax - 1, -2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(-2, lmax - 1), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(lmax - 1, 2), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked<int64_t>(2, lmax - 1), std::system_error);

                // This check is to foil the idea of comparing the result to check for overflow, wrongly assuming that a wrapped multiplication
                // violates monotonicity.
                // 5¹³ =  0x48C27395
                // 5¹⁴ = 0x16BCC41E9, which truncates to something still greater than 5¹³
            CHECK_THROWS_AS(slowmath::multiply_checked<int32_t>(0x48C27395L, 5), std::system_error);
            CHECK_THROWS_AS(slowmath::multiply_checked(uint32_t(0x48C27395UL), uint32_t(5)), std::system_error);
                // 3⁴⁰ =  0xA8B8B452291FE821
                // 3⁴¹ = 0x1FA2A1CF67B5FB863, which truncates to something still greater than 3⁴⁰
            CHECK_THROWS_AS(slowmath::multiply_checked(uint64_t(0xA8B8B452291FE821ULL), uint64_t(3)), std::system_error);
        }
    }

    SECTION("powi")
    {
        SECTION("good")
        {
            for (int b = 1; b <= 5; ++b)
            {
                CAPTURE(b);
                int r = 1;
                for (int e = 0; e <= 6; ++e)
                {
                    CAPTURE(e);
                    CHECK(slowmath::powi_checked(b, e) == r);
                    CHECK(int(slowmath::powi_checked(unsigned(b), unsigned(e))) == r);
                    r *= b;
                }
            }

            CHECK(slowmath::powi_checked(size_t(0), size_t(0)) == size_t(1));
            CHECK(slowmath::powi_checked(size_t(2), size_t(0)) == size_t(1));
            CHECK(slowmath::powi_checked(size_t(0), size_t(3)) == size_t(0));
            CHECK(slowmath::powi_checked(size_t(1), size_t(3)) == size_t(1));
            CHECK(slowmath::powi_checked(size_t(2), size_t(3)) == size_t(8));
            CHECK(slowmath::powi_checked(smax, size_t(1)) == smax);
            CHECK_NOTHROW(slowmath::powi_checked(size_t(2), smaxlog));

            CHECK(slowmath::powi_checked(0, 0) == 1);
            CHECK(slowmath::powi_checked(2, 0) == 1);
            CHECK(slowmath::powi_checked(0, 3) == 0);
            CHECK(slowmath::powi_checked(1, 3) == 1);
            CHECK(slowmath::powi_checked(2, 3) == 8);
            CHECK(slowmath::powi_checked(-1, 3) == -1);
            CHECK(slowmath::powi_checked(imin, 1) == imin);
            CHECK(slowmath::powi_checked(imax, 1) == imax);
            CHECK_NOTHROW(slowmath::powi_checked(2, imaxlog));
            CHECK(slowmath::powi_checked(-2, iminlog) == imin);
            CHECK_THROWS(slowmath::powi_checked(int8_t(-128), 2));
            CHECK(slowmath::powi_checked(int8_t(-2), 7) == int8_t(-128));
        }

        SECTION("bad")
        {
            CHECK_THROWS_AS(slowmath::powi_checked(size_t(2), smaxlog + 1), std::system_error);
            CHECK_THROWS_AS(slowmath::powi_checked(smax, size_t(2)), std::system_error);

            CHECK_THROWS_AS(slowmath::powi_checked(0, -1), gsl::fail_fast); // domain error
            CHECK_THROWS_AS(slowmath::powi_checked(2, imaxlog + 1), std::system_error);
            CHECK_THROWS_AS(slowmath::powi_checked(-2, iminlog + 1), std::system_error);
            CHECK_THROWS_AS(slowmath::powi_checked(imax, 2), std::system_error);
            CHECK_THROWS_AS(slowmath::powi_checked(imin, 2), std::system_error);

                // This check is to foil the idea of comparing the result to check for overflow, wrongly assuming that a wrapped multiplication
                // violates monotonicity.
                // 5¹³ =  0x48C27395
                // 5¹⁴ = 0x16BCC41E9, which truncates to something still greater than 5¹³
            CHECK(slowmath::powi_checked(uint32_t(5), uint32_t(13)) == 0x48C27395UL);
            CHECK_THROWS_AS(slowmath::powi_checked(uint32_t(5), uint32_t(14)), std::system_error);
                // 3⁴⁰ =  0xA8B8B452291FE821
                // 3⁴¹ = 0x1FA2A1CF67B5FB863, which truncates to something still greater than 3⁴⁰
                // This check also fails with a rounding error if the implementation uses floating-point numbers.
            CHECK(slowmath::powi_checked(uint64_t(3), uint64_t(40)) == 0xA8B8B452291FE821ULL);
            CHECK_THROWS_AS(slowmath::powi_checked(uint64_t(3), uint64_t(41)), std::system_error);
        }
    }

    SECTION("ceili")
    {
        CHECK(slowmath::ceili_checked(0, 3) == 0);
        CHECK(slowmath::ceili_checked(1, 3) == 3);
        CHECK(slowmath::ceili_checked(2, 3) == 3);
        CHECK(slowmath::ceili_checked(3, 3) == 3);
        CHECK(slowmath::ceili_checked(4, 3) == 6);

        CHECK(slowmath::ceili_checked(0u, 3u) == 0u);
        CHECK(slowmath::ceili_checked(1u, 3u) == 3u);
        CHECK(slowmath::ceili_checked(2u, 3u) == 3u);
        CHECK(slowmath::ceili_checked(3u, 3u) == 3u);
        CHECK(slowmath::ceili_checked(4u, 3u) == 6u);
    }
}
