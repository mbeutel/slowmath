
#ifndef INCLUDED_SLOWMATH_DETAIL_BITS_HPP_
#define INCLUDED_SLOWMATH_DETAIL_BITS_HPP_


#include <climits>      // for CHAR_BIT
#include <system_error> // for errc

#include <slowmath/detail/type_traits.hpp> // for max_v<>, integral_value_type<>, result_t<>


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning( push )
# pragma warning( disable: 4702 ) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{

namespace detail
{


template <typename EH, typename X, typename S>
constexpr result_t<EH, integral_value_type<X>> shift_right(X x, S s)
{
    using V0 = integral_value_type<X>;

    if (s >= gsl::narrow_cast<integral_value_type<S>>(sizeof(X) * CHAR_BIT)) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V0(x >> V0(s)));
}


template <typename EH, typename X, typename S>
constexpr result_t<EH, integral_value_type<X>> shift_left(X x, S s)
{
    using V0 = integral_value_type<X>;

    if (s >= gsl::narrow_cast<integral_value_type<S>>(sizeof(V0) * CHAR_BIT) || x > (max_v<V0> >> s)) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V0(x << V0(s)));
}


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
 #pragma warning( pop )
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_BITS_HPP_
