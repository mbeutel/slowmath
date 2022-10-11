
#include <tuple>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <slowmath/arithmetic.hpp>


TEST_CASE("unchecked operations tolerate unsigned overflow")
{
    static constexpr std::uint8_t u8m = std::numeric_limits<std::uint8_t>::max();
    static constexpr std::uint8_t u8mSq = std::uint8_t((u8m * u8m) & 0xFF);
    CHECK(slowmath::square(u8m) == u8mSq);
    CHECK_THROWS_AS(slowmath::square_checked(u8m), std::system_error);
}

TEMPLATE_TEST_CASE("absi(), negate() for signed types", "[arithmetic]", std::int8_t, std::int32_t, std::int64_t)
{
    static constexpr auto iMin = std::numeric_limits<TestType>::min();

    SECTION("basic correctness")
    {
        auto x = GENERATE(range(TestType(0), TestType(4)));

        CAPTURE(x);

        CHECK(slowmath::absi_checked(x) == x);
        CHECK(slowmath::absi_checked(TestType(-x)) == x);

        CHECK(slowmath::negate_checked(x) == -x);
        CHECK(slowmath::negate_checked(TestType(-x)) == x);
    }

    SECTION("throws on integer overflow")
    {
        CHECK_THROWS_AS(slowmath::absi_checked(iMin), std::system_error);
        CHECK_THROWS_AS(slowmath::negate_checked(iMin), std::system_error);
    }
}

TEMPLATE_TEST_CASE("absi(), negate() for unsigned types", "[arithmetic]", std::uint8_t, std::uint32_t, std::uint64_t)
{
    SECTION("zero")
    {
        CHECK(slowmath::absi_checked(TestType(0)) == TestType(0));
        CHECK(slowmath::negate_checked(TestType(0)) == TestType(0));
    }

    SECTION("basic correctness")
    {
        auto x = GENERATE(range(TestType(1), TestType(4)));

        CAPTURE(x);

        CHECK(slowmath::absi_checked(x) == x);
    }

    SECTION("throws on integer underflow")
    {
        auto x = GENERATE(range(TestType(1), TestType(4)));

        CAPTURE(x);

        CHECK_THROWS_AS(slowmath::negate_checked(x), std::system_error);
    }
}


// TODO: add comprehensive tests for add()
// TODO: add comprehensive tests for subtract()


TEMPLATE_TEST_CASE("multiply() for signed types", "[arithmetic]", std::int8_t, std::int32_t, std::int64_t)
{
    static constexpr auto iMin = std::numeric_limits<TestType>::min();
    static constexpr auto iMax = std::numeric_limits<TestType>::max();

    SECTION("basic correctness")
    {
        auto a = GENERATE(range(TestType(-7), TestType(8)));
        auto b = GENERATE(range(TestType(-7), TestType(8)));

        CAPTURE(a);
        CAPTURE(b);
        CHECK(slowmath::multiply_checked(a, b) == a * b);
    }

    SECTION("borderline values")
    {
        using Tuple = std::tuple<TestType, TestType>;
        auto a_b = GENERATE(
            Tuple{ iMin,               TestType( 0) },
            Tuple{ iMax,               TestType( 0) },
            Tuple{ iMin,               TestType( 1) },
            Tuple{ iMax,               TestType( 1) },
            Tuple{ iMax,               TestType(-1) },
            Tuple{ TestType(iMax / 2), TestType( 2) },
            Tuple{ TestType(iMax / 2), TestType(-2) },
            Tuple{ TestType(iMin / 2), TestType( 2) },
            Tuple{ TestType(iMin / 3), TestType(-2) }
        );
        TestType a, b;
        std::tie(a, b) = a_b;

        CAPTURE(a);
        CAPTURE(b);
        CHECK(slowmath::multiply_checked(a, b) == a * b);
        CHECK(slowmath::multiply_checked(b, a) == a * b);
    }

    SECTION("throws on integer overflow")
    {
        using Tuple = std::tuple<TestType, TestType>;
        auto a_b = GENERATE(
            Tuple{ iMin,               TestType( 2) },
            Tuple{ iMin,               TestType(-1) },
            Tuple{ iMin,               TestType(-2) },
            Tuple{ iMax,               TestType( 2) },
            Tuple{ iMax,               TestType(-2) },
            Tuple{ iMin,               TestType(-1) },
            Tuple{ TestType(iMax / 2), TestType( 3) },
            Tuple{ TestType(iMax / 2), TestType(-3) },
            Tuple{ TestType(iMin / 2), TestType( 3) },
            Tuple{ TestType(iMin / 2), TestType(-2) }
        );
        TestType a, b;
        std::tie(a, b) = a_b;

        CAPTURE(a);
        CAPTURE(b);
        CHECK_THROWS(slowmath::multiply_checked(a, b));
        CHECK_THROWS(slowmath::multiply_checked(b, a));
    }
}

TEMPLATE_TEST_CASE("multiply() for unsigned types", "[arithmetic]", std::uint8_t, std::uint32_t, std::uint64_t)
{
    static constexpr TestType iMax = std::numeric_limits<TestType>::max();

    SECTION("basic correctness")
    {
        auto a = GENERATE(range(TestType(0), TestType(8)));
        auto b = GENERATE(range(TestType(0), TestType(8)));

        CAPTURE(a);
        CAPTURE(b);
        CHECK(slowmath::multiply_checked(a, b) == a * b);
    }

    SECTION("borderline values")
    {
        using Tuple = std::tuple<TestType, TestType>;
        auto a_b = GENERATE(
            Tuple{ iMax,               TestType(0) },
            Tuple{ iMax,               TestType(1) },
            Tuple{ TestType(iMax / 2), TestType(2) }
        );
        TestType a, b;
        std::tie(a, b) = a_b;

        CAPTURE(a);
        CAPTURE(b);
        CHECK(slowmath::multiply_checked(a, b) == a * b);
        CHECK(slowmath::multiply_checked(b, a) == a * b);
    }

    SECTION("throws on integer overflow")
    {
        using Tuple = std::tuple<TestType, TestType>;
        auto a_b = GENERATE(
            Tuple{ iMax,               TestType(2) },
            Tuple{ TestType(iMax / 2), TestType(3) }
        );
        TestType a, b;
        std::tie(a, b) = a_b;

        CAPTURE(a);
        CAPTURE(b);
        CHECK_THROWS(slowmath::multiply_checked(a, b));
        CHECK_THROWS(slowmath::multiply_checked(b, a));
    }
}


// TODO: add comprehensive tests for divide()
// TODO: add comprehensive tests for modulo()
