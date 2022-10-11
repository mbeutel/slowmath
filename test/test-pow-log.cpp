
#include <tuple>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include <slowmath/arithmetic.hpp>


TEMPLATE_TEST_CASE("sqrti()", "[pow][log]", unsigned, int)
{
    static constexpr auto iMax = std::numeric_limits<TestType>::max();

    SECTION("enforces preconditions")
    {
        CHECK_THROWS(slowmath::detail::sqrti(-1));
        CHECK_THROWS(slowmath::detail::sqrti(-2));
    }

    SECTION("basic correctness")
    {
        auto x = GENERATE(range(TestType(0), TestType(100)));

        CAPTURE(x);

        auto sqrtX = slowmath::detail::sqrti(x);
        CHECK(sqrtX*sqrtX <= x);
        CHECK((sqrtX + 1)*(sqrtX + 1) > x);
    }

    SECTION("borderline values")
    {
        auto x = GENERATE(as<TestType>{ }, iMax / 2, iMax - 2, iMax - 1, iMax);

        CAPTURE(x);

        auto sqrtX = slowmath::detail::sqrti(x);
        CHECK(sqrtX*sqrtX <= x);
        CHECK(sqrtX*sqrtX > x - 2*sqrtX - 1); // equivalent to `(sqrtX + 1)*(sqrtX + 1) > x` but without the possibility of overflow
    }
}

TEMPLATE_TEST_CASE("square()", "[arithmetic]", int)
{
    static constexpr auto iMax = std::numeric_limits<TestType>::max();
    static constexpr auto sqrtIMax = slowmath::detail::sqrti(iMax);

    SECTION("basic correctness")
    {
        auto x = GENERATE(range(TestType(-12), TestType(13)), TestType(sqrtIMax - 1), sqrtIMax);

        CAPTURE(x);

        auto sqrX = slowmath::square_checked(x);
        CHECK(sqrX == x*x);
    }

    SECTION("throws on integer overflow")
    {
        int x = GENERATE(sqrtIMax + 1, sqrtIMax * 2);

        CAPTURE(x);

        CHECK_THROWS(slowmath::square_checked(x));
    }
}

TEMPLATE_TEST_CASE("log_floori(), log_ceili()", "[arithmetic]", int)
{
    static constexpr auto iMax = std::numeric_limits<TestType>::max();

    SECTION("basic correctness")
    {
        using Tuple = std::tuple<TestType, TestType, int, int>;
        auto x_b_logFloor_logCeil = GENERATE(
            Tuple{ 1,        2,        0,                                         0                                         },
            Tuple{ 2,        2,        1,                                         1                                         },
            Tuple{ 3,        2,        1,                                         2                                         },
            Tuple{ 8,        2,        3,                                         3                                         },
            Tuple{ iMax,     2,        TestType(sizeof(TestType) * CHAR_BIT - 2), TestType(sizeof(TestType) * CHAR_BIT) - 1 },
            Tuple{ iMax,     iMax,     1,                                         1                                         },
            Tuple{ iMax - 1, iMax,     0,                                         1                                         },
            Tuple{ iMax,     iMax - 1, 1,                                         2                                         },
            Tuple{ 1,        iMax,     0,                                         0                                         },
            Tuple{ 2,        iMax,     0,                                         1                                         }
        );
        TestType x, b;
        int logFloor, logCeil;
        std::tie(x, b, logFloor, logCeil) = x_b_logFloor_logCeil;
        
        CAPTURE(x);
        CAPTURE(b);

        CHECK(slowmath::log_floori<int>(x, b) == logFloor);
        CHECK(slowmath::log_ceili<int>(x, b) == logCeil);
    }
}

// TODO: add comprehensive tests for powi()
