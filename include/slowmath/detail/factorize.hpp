
#ifndef INCLUDED_SLOWMATH_DETAIL_FACTORIZE_HPP_
#define INCLUDED_SLOWMATH_DETAIL_FACTORIZE_HPP_


#include <slowmath/detail/arithmetic.hpp>   // for multiply<>()
#include <slowmath/detail/errors.hpp>       // for SLOWMATH_DETAIL_OVERFLOW_CHECK(), try_error_handler
#include <slowmath/detail/type_traits.hpp>  // for max_v<>, common_integral_value_type<>, integral_value_type<>, result_t<>, has_wider_type<>


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning(push)
# pragma warning(disable: 4702) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{


template <typename V, typename E, int NumFactors>
struct factorization;


namespace detail
{


    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, e) such that x = bᵉ + r with r ≥ 0 minimal.
template <typename E, typename X, typename B>
constexpr factorization<common_integral_value_type<X, B>, E, 1>
factorize_floori(X x, B b)
{
    using V = common_integral_value_type<X, B>;

    E e = 0;
    V x0 = 1;
    constexpr V M = max_v<V>;
    V m = M / b;

        // We generally assume bᵉ ≤ x (⇔ e ≤ log x ÷ log b).
        //
        // Then, by definition,
        //
        //  (    bᵉ⁺¹ > x
        //    ⇔ e+1 > log x ÷ log b
        //    ⇒ e = ⌊log x ÷ log b⌋ )   (<-- #1)
        //
        // Additionally we know that
        //
        //  (    bᵉ > m = ⌊M ÷ b⌋
        //    ⇒ bᵉ > M ÷ b
        //    ⇒ bᵉ⁺¹ > M ≥ x
        //    ⇒ e = ⌊log x ÷ log b⌋ )   because of #1 (<-- #2)

        // x₀ ≤ x upon loop entry because x₀ = 1, x ≥ 1.
    for (;;)
    {
            // Compare with m before computing bᵉ⁺¹ to avoid overflow.
        if (x0 > m)
        {
            return { x - x0, e };
        }

        V x1 = x0 * b; // = bᵉ⁺¹
        if (x1 > x)
        {
            return { x - x0, e };
        }

        x0 = x1;
        ++e;
    }
}


    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, e) such that x = bᵉ - r with r ≥ 0 minimal.
template <typename EH, typename E, typename X, typename B>
constexpr result_t<EH, factorization<common_integral_value_type<X, B>, E, 1>>
factorize_ceili(X x, B b)
{
    using V = common_integral_value_type<X, B>;

    auto floorFac = detail::factorize_floori<E>(x, b);
    if (floorFac.remainder == 0)
    {
        return EH::make_result(floorFac);
    }

    auto xFloor = x - floorFac.remainder; // = bᵉ
    auto prodResult = detail::multiply<EH>(xFloor, b - 1);
    if (EH::is_error(prodResult)) return EH::passthrough_error(prodResult);
    auto prod = EH::get_value(prodResult);

    auto rCeil = prod - floorFac.remainder; // x = bᵉ + r =: bᵉ⁺¹ - r' ⇒ r' = bᵉ(b - 1) - r
    return EH::make_result(factorization<V, E, 1>{ rCeil, floorFac.exponent1 + 1 }); // e cannot overflow
}


    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r, i, j) such that x = aⁱ ∙ bʲ + r with r ≥ 0 minimal.
template <typename E, typename X, typename A, typename B>
constexpr factorization<common_integral_value_type<X, A, B>, E, 2>
factorize_floori(X x, A a, B b)
{
    using V = common_integral_value_type<X, A, B>;

        // adaption of algorithm in factorize_ceili() for different optimization criterion

    auto facA = detail::factorize_floori<E>(x, a);
        
    E i = facA.exponent1,
      j = 0;
    E ci = i,
      cj = j;
    V y = x - facA.remainder; // = aⁱ
    V cy = y; // cy ≤ x at all times

    for (;;)
    {
        if (i == 0)
        {
            return { x - cy, ci, cj };
        }

            // take factor a
        y /= a;
        --i;

            // give factors b as long as y ≤ x
            // (note that y ∙ b overflowing implies y ∙ b > x)
        for (;;)
        {
            auto ybResult = detail::multiply<try_error_handler>(y, b);
            if (ybResult.ec != std::errc{ } || ybResult.value > x) break;
            y = ybResult.value;
            ++j;
        }

        if (y > cy)
        {
            cy = y;
            ci = i;
            cj = j;
        }
    }
}


    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r, i, j) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
template <typename EH, typename E, typename X, typename A, typename B>
constexpr result_t<EH, factorization<common_integral_value_type<X, A, B>, E, 2>>
factorize_ceili(X x, A a, B b)
{
    using V = common_integral_value_type<X, A, B>;

        // algorithm discussed in http://stackoverflow.com/a/39050139 and slightly altered to avoid unnecessary overflows

    auto facAResult = detail::factorize_ceili<EH, E>(x, a);
    if (EH::is_error(facAResult)) return EH::passthrough_error(facAResult);
    auto facA = EH::get_value(facAResult);

    SLOWMATH_DETAIL_OVERFLOW_CHECK(x <= max_v<V> - facA.remainder);
    V y0 = x + facA.remainder;

    E i = facA.exponent1,
      j = 0;
    E ci = i,
      cj = j;
    V y = y0;
    V cy = y; // cy ≥ x at all times

    for (;;)
    {
        if (i == 0)
        {
            return EH::make_result(factorization<V, E, 2>{ cy - x, ci, cj });
        }

            // take factor a
        y /= a;
        --i;

            // give factors b until y ≥ x
        while (y < x)
        {
            auto yResult = detail::multiply<EH>(y, b);
            if (EH::is_error(yResult)) return EH::passthrough_error(yResult);
            y = EH::get_value(yResult);
            ++j;
        }

        if (y < cy)
        {
            cy = y;
            ci = i;
            cj = j;
        }
    }
}


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning(pop)
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_FACTORIZE_HPP_
