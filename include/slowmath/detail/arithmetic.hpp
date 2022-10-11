
#ifndef INCLUDED_SLOWMATH_DETAIL_ARITHMETIC_HPP_
#define INCLUDED_SLOWMATH_DETAIL_ARITHMETIC_HPP_


#include <type_traits>  // for integral_constant<>, make_unsigned<>, is_signed<>

#include <slowmath/detail/type_traits.hpp>  // for min_v<>, max_v<>, common_integral_value_type<>, integral_value_type<>, result_t<>, has_wider_type<>
#include <slowmath/detail/errors.hpp>       // for SLOWMATH_DETAIL_OVERFLOW_CHECK()


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning(push)
# pragma warning(disable: 4127) // conditional expression is constant
# pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned
# pragma warning(disable: 4702) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{

namespace detail
{


    // The implementations below have borrowed heavily from the suggestions made and examples used in the SEI CERT C Coding Standard:
    // https://wiki.sei.cmu.edu/confluence/display/c/


template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>>
absi(V v)
{
    using V0 = integral_value_type<V>;
    using S = std::make_signed_t<V0>;

    if (std::is_signed<V0>::value) // should be `if constexpr` in C++17
    {
            // This assumes a two's complement representation.
        SLOWMATH_DETAIL_OVERFLOW_CHECK(v != min_v<V0>);
        return EH::make_result(V0(S(v) < 0 ? -v : v)); // cast to signed to avoid warning about pointless unsigned comparison
    }
    else
    {
        return EH::make_result(V0(v));
    }
}


template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>>
negate(V v)
{
    using V0 = integral_value_type<V>;

    if (std::is_signed<V0>::value) // should be `if constexpr` in C++17
    {
            // This assumes a two's complement representation.
        SLOWMATH_DETAIL_OVERFLOW_CHECK(v != min_v<V0>);
    }
    else
    {
        SLOWMATH_DETAIL_OVERFLOW_CHECK(v == 0);
    }
    return EH::make_result(V0(-v));
}


template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
add_narrow(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using UV = std::make_unsigned_t<V>;
    using W = wider_type<V>;
    using UW = std::make_unsigned_t<W>;

        // This assumes a two's complement representation.
    W result = W(a) + W(b);
    SLOWMATH_DETAIL_OVERFLOW_CHECK(UW(result - W(min_v<V>)) <= max_v<UV>);
    return EH::make_result(V(result));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
add_wide(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using S = std::make_signed_t<V>;
    using U = std::make_unsigned_t<V>;

    V result = V(U(a) + U(b));
    if (std::is_signed<V>::value) // should be `if constexpr` in C++17
    {
            // cast to signed to avoid warning about pointless unsigned comparison
        SLOWMATH_DETAIL_OVERFLOW_CHECK(
            !(S(a) < 0 && S(b) < 0 && S(result) >= 0)
         && !(S(a) > 0 && S(b) > 0 && S(result) <  0));
    }
    else
    {
        SLOWMATH_DETAIL_OVERFLOW_CHECK(result >= a && result >= b);
    }
    return EH::make_result(result);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
add_0(std::true_type /*isNarrow*/, A a, B b)
{
    return detail::add_narrow<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
add_0(std::false_type /*isNarrow*/, A a, B b)
{
    return detail::add_wide<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
add(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    return detail::add_0<EH>(has_wider_type<V>{ }, a, b);
}


template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
subtract(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using S = std::make_signed_t<V>;

    if (std::is_signed<V>::value) // should be `if constexpr` in C++17
    {
            // cast to signed to avoid warning about pointless unsigned comparison
        SLOWMATH_DETAIL_OVERFLOW_CHECK(
            !(S(b) > 0 && a < min_v<V> + b)
         && !(S(b) < 0 && a > max_v<V> + b));
    }
    else
    {
        SLOWMATH_DETAIL_OVERFLOW_CHECK(a >= b);
    }
    return EH::make_result(V(a - b));
}


template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
multiply_narrow(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using UV = std::make_unsigned_t<V>;
    using W = wider_type<V>;
    using UW = std::make_unsigned_t<W>;

        // This assumes a two's complement representation.
    W result = W(a) * W(b);
    SLOWMATH_DETAIL_OVERFLOW_CHECK(UW(result - W(min_v<V>)) <= max_v<UV>);
    return EH::make_result(V(result));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
multiply_wide(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using S = std::make_signed_t<V>;

    if (std::is_signed<V>::value) // should be `if constexpr` in C++17
    {
            // cast to signed to avoid warning about pointless unsigned comparison
        SLOWMATH_DETAIL_OVERFLOW_CHECK(
            !(S(a) > 0 && ((S(b) > 0 && S(a) > S(max_v<V> / b)) || (S(b) <= 0 && S(b < min_v<V> / a))))
         && !(S(a) < 0 && ((S(b) > 0 && S(a) < S(min_v<V> / b)) || (S(b) <= 0 && S(b < max_v<V> / a)))));
    }
    else
    {
        SLOWMATH_DETAIL_OVERFLOW_CHECK(!(b > 0 && a > max_v<V> / b));
    }
    return EH::make_result(V(a * b));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
multiply_0(std::true_type /*isNarrow*/, A a, B b)
{
    return detail::multiply_narrow<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
multiply_0(std::false_type /*isNarrow*/, A a, B b)
{
    return detail::multiply_wide<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>>
multiply(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    return detail::multiply_0<EH>(has_wider_type<V>{ }, a, b);
}


template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>>
divide(N n, D d)
{
    using V = common_integral_value_type<N, D>;

    if (std::is_signed<V>::value) // should be `if constexpr` in C++17
    {
        SLOWMATH_DETAIL_OVERFLOW_CHECK(!(n == min_v<V> && d == -1));
    }
    return EH::make_result(V(n / d));
}


template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>>
modulo(N n, D d)
{
    using V = common_integral_value_type<N, D>;

    if (std::is_signed<V>::value) // should be `if constexpr` in C++17
    {
        SLOWMATH_DETAIL_OVERFLOW_CHECK(!(n == min_v<V> && d == -1));
    }
    return EH::make_result(V(n % d));
}


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning(pop)
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_ARITHMETIC_HPP_
