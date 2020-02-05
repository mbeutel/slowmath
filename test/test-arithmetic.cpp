
#include <tuple>

#include <catch2/catch.hpp>

#include <slowmath/arithmetic.hpp>


TEMPLATE_TEST_CASE("absi(), negate() for signed types", "[arithmetic]", std::int8_t, std::int32_t, std::int64_t)
{
    static constexpr auto iMin = std::numeric_limits<TestType>::min();

    SECTION("basic correctness")
    {
        auto x = GENERATE(range(TestType(0), TestType(4)));

        CAPTURE(x);

        CHECK(slowmath::absi(x) == x);
        CHECK(slowmath::absi(TestType(-x)) == x);

        CHECK(slowmath::negate(x) == -x);
        CHECK(slowmath::negate(TestType(-x)) == x);
    }

    SECTION("throws on integer overflow")
    {
        CHECK_THROWS_AS(slowmath::absi(iMin), std::system_error);
        CHECK_THROWS_AS(slowmath::negate(iMin), std::system_error);
    }
}

TEMPLATE_TEST_CASE("absi(), negate() for unsigned types", "[arithmetic]", std::uint8_t, std::uint32_t, std::uint64_t)
{
    SECTION("zero")
    {
        CHECK(slowmath::absi(TestType(0)) == TestType(0));
        CHECK(slowmath::negate(TestType(0)) == TestType(0));
    }

    SECTION("basic correctness")
    {
        auto x = GENERATE(range(TestType(1), TestType(4)));

        CAPTURE(x);

        CHECK(slowmath::absi(x) == x);
    }

    SECTION("throws on integer underflow")
    {
        auto x = GENERATE(range(TestType(1), TestType(4)));

        CAPTURE(x);

        CHECK_THROWS_AS(slowmath::negate(x), std::system_error);
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
        CHECK(slowmath::multiply(a, b) == a * b);
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
        CHECK(slowmath::multiply(a, b) == a * b);
        CHECK(slowmath::multiply(b, a) == a * b);
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
        CHECK_THROWS(slowmath::multiply(a, b));
        CHECK_THROWS(slowmath::multiply(b, a));
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
        CHECK(slowmath::multiply(a, b) == a * b);
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
        CHECK(slowmath::multiply(a, b) == a * b);
        CHECK(slowmath::multiply(b, a) == a * b);
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
        CHECK_THROWS(slowmath::multiply(a, b));
        CHECK_THROWS(slowmath::multiply(b, a));
    }
}


// TODO: add comprehensive tests for divide()
// TODO: add comprehensive tests for modulo()
