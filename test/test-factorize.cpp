
#include <tuple>

#include <catch2/catch.hpp>

#include <slowmath/arithmetic.hpp>


// TODO: add comprehensive tests for single-factor overloads of factorize_floori() and factorize_ceili()

TEMPLATE_TEST_CASE("factorize_floori(), factorize_ceili()", "[factorize]", int)
{
    SECTION("basic correctness")
    {
        using Tuple = std::tuple<TestType, TestType, TestType, int, int, TestType, int, int, TestType>;
        auto x_a_b_floorExpA_floorExpB_floorR_ceilExpA_ceilExpB_ceilR = GENERATE(
            Tuple{ 1, 2, 3,  0, 0, 0,  0, 0, 0 },
            Tuple{ 2, 2, 3,  1, 0, 0,  1, 0, 0 }, // 2 = 2^1           = 2^1
            Tuple{ 3, 2, 3,  0, 1, 0,  0, 1, 0 }, // 3 =       3^1     =       3^1    
            Tuple{ 4, 2, 3,  2, 0, 0,  2, 0, 0 }, // 4 = 2^2           = 2^2
            Tuple{ 5, 2, 3,  2, 0, 1,  1, 1, 1 }, // 5 = 2^2       + 1 = 2^1 + 3^1 - 1
            Tuple{ 6, 2, 3,  1, 1, 0,  1, 1, 0 }, // 6 = 2^1 + 3^1     = 2^1 + 3^1    
            Tuple{ 7, 2, 3,  1, 1, 1,  3, 0, 1 }, // 7 = 2^1 + 3^1 + 1 = 2^3       - 1
            Tuple{ 8, 2, 3,  3, 0, 0,  3, 0, 0 }  // 8 = 2^3           = 2^3          
        );
        int floorExpA, floorExpB, ceilExpA, ceilExpB;
        TestType x, a, b, floorR, ceilR;
        std::tie(x, a, b, floorExpA, floorExpB, floorR, ceilExpA, ceilExpB, ceilR) = x_a_b_floorExpA_floorExpB_floorR_ceilExpA_ceilExpB_ceilR;
        
        CAPTURE(x);
        CAPTURE(a);
        CAPTURE(b);

        CHECK(slowmath::factorize_floori<int>(x, a, b) == slowmath::factorization<TestType, int, 2>{ floorR, floorExpA, floorExpB });
        CHECK(slowmath::factorize_ceili_checked<int>(x, a, b) == slowmath::factorization<TestType, int, 2>{ ceilR, ceilExpA, ceilExpB });
    }

    // TODO: add borderline and overflow tests
}
