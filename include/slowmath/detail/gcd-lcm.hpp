
#ifndef INCLUDED_SLOWMATH_DETAIL_GCD_LCM_HPP_
#define INCLUDED_SLOWMATH_DETAIL_GCD_LCM_HPP_


#include <numeric>      // for gcd(), lcm()
#include <type_traits>  // for is_signed<>
#include <system_error> // for errc

#include <gsl/gsl-lite.hpp> // for gsl_CPP17_OR_GREATER

#include <slowmath/detail/type_traits.hpp> // for min_v<>, common_integral_value_type<>, integral_value_type<>, result_t<>


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning( push )
# pragma warning( disable: 4702 ) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{

namespace detail
{


#if gsl_CPP17_OR_GREATER
    // Computes the greatest common divisor of a and b.
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> gcd_unsigned(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    return EH::make_result(V(std::gcd(integral_value_type<A>(a), integral_value_type<B>(b))));
}
    // Computes the greatest common divisor of a and b.
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> gcd_signed(A a, B b)
{
    using V = common_integral_value_type<A, B>;

        // This assumes a two's complement representation (it will yield a false negative for one's complement integers).
    if (a == min_v<V> || b == min_v<V>) return EH::make_error(std::errc::value_too_large);

    return EH::make_result(V(std::gcd(integral_value_type<A>(a), integral_value_type<B>(b))));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> gcd(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if constexpr (std::is_signed_v<V>) return detail::gcd_signed<EH>(a, b);
    else return detail::gcd_unsigned<EH>(a, b);
}


    // Computes the least common multiple of a and b.
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> lcm_unsigned(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if (a == 0 || b == 0) return EH::make_result(V(0));

    V lgcd = std::gcd(integral_value_type<A>(a), integral_value_type<B>(b));

    return detail::multiply<EH>(a / lgcd, b);
}
    // Computes the least common multiple of a and b.
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> lcm_signed(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if (a == 0 || b == 0) return EH::make_result(V(0));

        // This assumes a two's complement representation (it will yield a false negative for one's complement integers).
    if (a == min_v<V> || b == min_v<V>) return EH::make_error(std::errc::value_too_large);

    V av = a < 0 ? -a : a;
    V bv = b < 0 ? -b : b;
    V lgcd = std::gcd(av, bv);

    return detail::multiply<EH>(a / lgcd, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> lcm(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if constexpr (std::is_signed_v<V>) return detail::lcm_signed<EH>(a, b);
    else return detail::lcm_unsigned<EH>(a, b);
}
#endif // gsl_CPP17_OR_GREATER


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
 #pragma warning( pop )
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_GCD_LCM_HPP_