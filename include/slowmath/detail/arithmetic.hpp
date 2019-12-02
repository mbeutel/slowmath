
#ifndef INCLUDED_SLOWMATH_DETAIL_ARITHMETIC_HPP_
#define INCLUDED_SLOWMATH_DETAIL_ARITHMETIC_HPP_


#include <type_traits>  // for integral_constant<>, make_unsigned<>, is_signed<>
#include <system_error> // for errc

#include <gsl/gsl-lite.hpp> // for Expects(), gsl_CPP17_OR_GREATER

#include <slowmath/detail/type_traits.hpp> // for min_v<>, max_v<>, common_integral_value_type<>, integral_value_type<>, result_t<>, has_wider_type<>


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning( push )
# pragma warning( disable: 4702 ) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{

namespace detail
{


    // The implementations below have borrowed heavily from the suggestions made and examples used in the SEI CERT C Coding Standard:
    // https://wiki.sei.cmu.edu/confluence/display/c/


template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> absi_unsigned(V v)
{
    using V0 = integral_value_type<V>;

    return EH::make_result(V0(v));
}
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> absi_signed(V v)
{
    using V0 = integral_value_type<V>;

        // This assumes a two's complement representation (it will yield a false negative for one's complement integers).
    if (v == min_v<V0>) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V0(v < 0 ? -v : v));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> absi_0(std::false_type /*isSigned*/, V v)
{
    return detail::absi_unsigned<EH>(v);
}
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> absi_0(std::true_type /*isSigned*/, V v)
{
    return detail::absi_signed<EH>(v);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> absi(V v)
{
    using V0 = integral_value_type<V>;

#if gsl_CPP17_OR_GREATER
    if constexpr (std::is_signed_v<V0>) return detail::absi_signed<EH>(v);
    else return detail::absi_unsigned<EH>(v);
#else // gsl_CPP17_OR_GREATER
    return detail::absi_0<EH>(std::is_signed<V0>{ }, v);
#endif // gsl_CPP17_OR_GREATER
}

template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> negate_unsigned(V v)
{
    using V0 = integral_value_type<V>;

    if (v != 0) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V0(0));
}
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> negate_signed(V v)
{
    using V0 = integral_value_type<V>;

        // This assumes a two's complement representation (it will yield a false negative for one's complement integers).
    if (v == min_v<V0>) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V0(-v));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> negate_0(std::false_type /*isSigned*/, V v)
{
    return detail::negate_unsigned<EH>(v);
}
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> negate_0(std::true_type /*isSigned*/, V v)
{
    return detail::negate_signed<EH>(v);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> negate(V v)
{
    using V0 = integral_value_type<V>;

#if gsl_CPP17_OR_GREATER
    if constexpr (std::is_signed_v<V0>) return detail::negate_signed<EH>(v);
    else return detail::negate_unsigned<EH>(v);
#else // gsl_CPP17_OR_GREATER
    return detail::negate_0<EH>(std::is_signed<V0>{ }, v);
#endif // gsl_CPP17_OR_GREATER
}


template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> add_narrow(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using W = wider_type<V>;

    W result = W(a) + W(b);
    if (result < min_v<V> || result > max_v<V>) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(result));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> add_wide_unsigned(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    V result = a + b;
    if (result < a || result < b) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(result));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> add_wide_signed(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using U = std::make_unsigned_t<V>;

    V result = V(U(a) + U(b));
    if ((a <  0 && b <  0 && result >= 0)
     || (a >= 0 && b >= 0 && result <  0))
    {
        return EH::make_error(std::errc::value_too_large);
    }
    return EH::make_result(V(result));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename BC, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> add_0(std::true_type /*isNarrow*/, BC /*isSigned*/, A a, B b)
{
    return detail::add_narrow<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> add_0(std::false_type /*isNarrow*/, std::false_type /*isSigned*/, A a, B b)
{
    return detail::add_wide_unsigned<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> add_0(std::false_type /*isNarrow*/, std::true_type /*isSigned*/, A a, B b)
{
    return detail::add_wide_signed<EH>(a, b);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> add(A a, B b)
{
    using V = common_integral_value_type<A, B>;

#if gsl_CPP17_OR_GREATER
    if constexpr (has_wider_type_v<V>) return detail::add_narrow<EH>(a, b);
    else if constexpr (std::is_signed_v<V>) return detail::add_wide_signed<EH>(a, b);
    else return detail::add_wide_unsigned<EH>(a, b);
#else // gsl_CPP17_OR_GREATER
    return detail::add_0<EH>(has_wider_type<V>{ }, std::is_signed<V>{ }, a, b);
#endif // gsl_CPP17_OR_GREATER
}


template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> subtract_unsigned(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if (a < b) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(a - b));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> subtract_signed(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if ((b > 0 && a < min_v<V> + b)
     || (b < 0 && a > max_v<V> + b))
    {
        return EH::make_error(std::errc::value_too_large);
    }
    return EH::make_result(V(a - b));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> subtract_0(std::false_type /*isSigned*/, A a, B b)
{
    return detail::subtract_unsigned<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> subtract_0(std::true_type /*isSigned*/, A a, B b)
{
    return detail::subtract_signed<EH>(a, b);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> subtract(A a, B b)
{
    using V = common_integral_value_type<A, B>;

#if gsl_CPP17_OR_GREATER
    if constexpr (std::is_signed_v<V>) return detail::subtract_signed<EH>(a, b);
    else return detail::subtract_unsigned<EH>(a, b);
#else // gsl_CPP17_OR_GREATER
    return detail::subtract_0<EH>(std::is_signed<V>{ }, a, b);
#endif // gsl_CPP17_OR_GREATER
}


template <typename A, typename B>
constexpr bool can_multiply_narrow(A a, B b) noexcept
{
    using V = common_integral_value_type<A, B>;
    using W = wider_type<V>;

    W result = W(a) * W(b);
    return result >= min_v<V> && result <= max_v<V>;
}
template <typename A, typename B>
constexpr bool can_multiply_wide_unsigned(A a, B b) noexcept
{
    using V = common_integral_value_type<A, B>;

    return !(b > 0 && a > max_v<V> / b);
}
template <typename A, typename B>
constexpr bool can_multiply_wide_signed(A a, B b) noexcept
{
    using V = common_integral_value_type<A, B>;

    return !(   (a > 0 && ((b > 0 && a > max_v<V> / b) || (b <= 0 && b < min_v<V> / a)))
             || (a < 0 && ((b > 0 && a < min_v<V> / b) || (b <= 0 && b < max_v<V> / a))));
}
#if !gsl_CPP17_OR_GREATER
template <typename BC, typename A, typename B>
constexpr bool can_multiply_0(std::true_type /*isNarrow*/, BC /*isSigned*/, A a, B b)
{
    return detail::can_multiply_narrow(a, b);
}
template <typename A, typename B>
    constexpr bool can_multiply_0(std::false_type /*isNarrow*/, std::false_type /*isSigned*/, A a, B b)
{
    return detail::can_multiply_wide_unsigned(a, b);
}
template <typename A, typename B>
    constexpr bool can_multiply_0(std::false_type /*isNarrow*/, std::true_type /*isSigned*/, A a, B b)
{
    return detail::can_multiply_wide_signed(a, b);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename A, typename B>
constexpr bool can_multiply(A a, B b)
{
    using V = common_integral_value_type<A, B>;

#if gsl_CPP17_OR_GREATER
    if constexpr (has_wider_type_v<V>) return detail::can_multiply_narrow(a, b);
    else if constexpr (std::is_signed_v<V>) return detail::can_multiply_wide_signed(a, b);
    else return detail::can_multiply_wide_unsigned(a, b);
#else // gsl_CPP17_OR_GREATER
    return detail::can_multiply_0(has_wider_type<V>{ }, std::is_signed<V>{ }, a, b);
#endif // gsl_CPP17_OR_GREATER
}

template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> multiply_narrow(A a, B b)
{
    using V = common_integral_value_type<A, B>;
    using W = wider_type<V>;

    W result = W(a) * W(b);
    if (result < min_v<V> || result > max_v<V>) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(result));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> multiply_wide_unsigned(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if (!detail::can_multiply_wide_unsigned(a, b)) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(a * b));
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> multiply_wide_signed(A a, B b)
{
    using V = common_integral_value_type<A, B>;

    if (!detail::can_multiply_wide_signed(a, b)) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(a * b));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename BC, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> multiply_0(std::true_type /*isNarrow*/, BC /*isSigned*/, A a, B b)
{
    return detail::multiply_narrow<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> multiply_0(std::false_type /*isNarrow*/, std::false_type /*isSigned*/, A a, B b)
{
    return detail::multiply_wide_unsigned<EH>(a, b);
}
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> multiply_0(std::false_type /*isNarrow*/, std::true_type /*isSigned*/, A a, B b)
{
    return detail::multiply_wide_signed<EH>(a, b);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename A, typename B>
constexpr result_t<EH, common_integral_value_type<A, B>> multiply(A a, B b)
{
    using V = common_integral_value_type<A, B>;

#if gsl_CPP17_OR_GREATER
    if constexpr (has_wider_type_v<V>) return detail::multiply_narrow<EH>(a, b);
    else if constexpr (std::is_signed_v<V>) return detail::multiply_wide_signed<EH>(a, b);
    else return detail::multiply_wide_unsigned<EH>(a, b);
#else // gsl_CPP17_OR_GREATER
    return detail::multiply_0<EH>(has_wider_type<V>{ }, std::is_signed<V>{ }, a, b);
#endif // gsl_CPP17_OR_GREATER
}


template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> divide_unsigned(N n, D d)
{
    using V = common_integral_value_type<N, D>;

    Expects(d != 0);

    return EH::make_result(V(n / d));
}
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> divide_signed(N n, D d)
{
    using V = common_integral_value_type<N, D>;

    Expects(d != 0);

    if (n == min_v<V> && d == -1) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(n / d));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> divide_0(std::false_type /*isSigned*/, N n, D d)
{
    return detail::divide_unsigned<EH>(n, d);
}
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> divide_0(std::true_type /*isSigned*/, N n, D d)
{
    return detail::divide_signed<EH>(n, d);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> divide(N n, D d)
{
    using V = common_integral_value_type<N, D>;

#if gsl_CPP17_OR_GREATER
    if constexpr (std::is_signed_v<V>) return detail::divide_signed<EH>(n, d);
    else return detail::divide_unsigned<EH>(n, d);
#else // gsl_CPP17_OR_GREATER
    return detail::divide_0<EH>(std::is_signed<V>{ }, n, d);
#endif // gsl_CPP17_OR_GREATER
}


template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> modulo_unsigned(N n, D d)
{
    using V = common_integral_value_type<N, D>;

    Expects(d != 0);

    return EH::make_result(V(n % d));
}
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> modulo_signed(N n, D d)
{
    using V = common_integral_value_type<N, D>;

    Expects(d != 0);

    if (n == min_v<V> && d == -1) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(n % d));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> modulo_0(std::false_type /*isSigned*/, N n, D d)
{
    return detail::modulo_unsigned<EH>(n, d);
}
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> modulo_0(std::true_type /*isSigned*/, N n, D d)
{
    return detail::modulo_signed<EH>(n, d);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename N, typename D>
constexpr result_t<EH, common_integral_value_type<N, D>> modulo(N n, D d)
{
    using V = common_integral_value_type<N, D>;

#if gsl_CPP17_OR_GREATER
    if constexpr (std::is_signed_v<V>) return detail::modulo_signed<EH>(n, d);
    else return detail::modulo_unsigned<EH>(n, d);
#else // gsl_CPP17_OR_GREATER
    return detail::modulo_0<EH>(std::is_signed<V>{ }, n, d);
#endif // gsl_CPP17_OR_GREATER
}


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
 #pragma warning( pop )
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_ARITHMETIC_HPP_
