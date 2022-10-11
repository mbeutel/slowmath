
#ifndef INCLUDED_SLOWMATH_DETAIL_ROUND_HPP_
#define INCLUDED_SLOWMATH_DETAIL_ROUND_HPP_


#include <slowmath/detail/type_traits.hpp>  // for max_v<>, common_integral_value_type<>, result_t<>
#include <slowmath/detail/errors.hpp>       // for SLOWMATH_DETAIL_OVERFLOW_CHECK()


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning(push)
# pragma warning(disable: 4702) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{

namespace detail
{


    // Computes ⌊x ÷ d⌋ ∙ d for x ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
template <typename X, typename D>
constexpr common_integral_value_type<X, D>
floori(X x, D d)
{
    return x - x % d;
}


    // Computes ⌈x ÷ d⌉ ∙ d for x ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
template <typename EH, typename X, typename D>
constexpr result_t<EH, common_integral_value_type<X, D>>
ceili(X x, D d)
{
    using V = common_integral_value_type<X, D>;

        // We have the following identities:
        //
        //     x = ⌊x ÷ d⌋ ∙ d + x mod d
        //     ⌈x ÷ d⌉ = ⌊(x + d - 1) ÷ d⌋ = ⌊(x - 1) ÷ d⌋ + 1
        //
        // Assuming x ≠ 0, we can derive the form
        //
        //     ⌈x ÷ d⌉ ∙ d = x + d - (x - 1) mod d - 1 .

    if (x == 0) return EH::make_result(V(0));
    V dx = d - (x - 1) % d - 1;
    SLOWMATH_DETAIL_OVERFLOW_CHECK(x <= max_v<V> - dx);
    return EH::make_result(V(x + dx));
}


    // Computes ⌊n ÷ d⌋ for n ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
template <typename N, typename D>
constexpr common_integral_value_type<N, D>
ratio_floori(N n, D d)
{
    return n / d;
}


    // Computes ⌈n ÷ d⌉ for n ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
template <typename N, typename D>
constexpr common_integral_value_type<N, D>
ratio_ceili(N n, D d)
{
    return n != 0
        ? (n - 1) / d + 1 // overflow-safe
        : 0;
}


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning(pop)
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_ROUND_HPP_
