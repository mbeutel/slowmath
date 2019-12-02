
#include <tuple>

#include <catch2/catch.hpp>

#include <slowmath/arithmetic.hpp>


TEMPLATE_TEST_CASE("floori(), ceili(), ratio_floori(), ratio_ceili() for signed types", "[round]", int)
{
    static constexpr auto iMax = std::numeric_limits<TestType>::max();

    SECTION("enforces preconditions")
    {
        using Tuple = std::tuple<TestType, TestType>;
        auto n_d = GENERATE(
            Tuple{ -1,  1 },
            Tuple{  1, -1 },
            Tuple{ -2,  1 },
            Tuple{ -2, -2 }
        );
        TestType n, d;
        std::tie(n, d) = n_d;

        CAPTURE(n);
        CAPTURE(d);

        CHECK_THROWS(slowmath::floori(n, d));
        CHECK_THROWS(slowmath::ceili(n, d));
        CHECK_THROWS(slowmath::ratio_floori(n, d));
        CHECK_THROWS(slowmath::ratio_ceili(n, d));
    }

}

TEMPLATE_TEST_CASE("floori(), ceili(), ratio_floori(), ratio_ceili()", "[round]", unsigned, int)
{
    static constexpr auto iMax = std::numeric_limits<TestType>::max();

    SECTION("enforces preconditions")
    {
        using Tuple = std::tuple<TestType, TestType>;
        auto n_d = GENERATE(
            Tuple{  0, 0 },
            Tuple{  1, 0 },
            Tuple{  2, 0 }
        );
        TestType n, d;
        std::tie(n, d) = n_d;

        CAPTURE(n);
        CAPTURE(d);

        CHECK_THROWS(slowmath::floori(n, d));
        CHECK_THROWS(slowmath::ceili(n, d));
        CHECK_THROWS(slowmath::ratio_floori(n, d));
        CHECK_THROWS(slowmath::ratio_ceili(n, d));
    }

    SECTION("basic correctness")
    {
        auto n = GENERATE(range(TestType(0), TestType(8)));
        auto d = GENERATE(range(TestType(1), TestType(8)));

        CAPTURE(n);
        CAPTURE(d);

        CHECK(slowmath::floori(n, d) == n / d * d);
        CHECK(slowmath::ceili(n, d) == (n + d - 1) / d * d);
        CHECK(slowmath::ratio_floori(n, d) == n / d);
        CHECK(slowmath::ratio_ceili(n, d) == (n + d - 1) / d);
    }

    SECTION("borderline values")
    {
        static_assert(iMax % 2 != 0, "iMax must be odd in a two's complement representation");
        CHECK(slowmath::floori(iMax, TestType(2)) == iMax - 1);
        CHECK(slowmath::ratio_floori(iMax, TestType(2)) == iMax / 2);
        CHECK(slowmath::ratio_ceili(iMax, TestType(2)) == iMax / 2 + 1);
    }

    SECTION("throws on integer overflow")
    {
        static_assert(iMax % 2 != 0, "iMax must be odd in a two's complement representation");
        CHECK_THROWS(slowmath::ceili(iMax, TestType(2)));
    }
}
