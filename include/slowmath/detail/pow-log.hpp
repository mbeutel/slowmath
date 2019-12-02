
#ifndef INCLUDED_SLOWMATH_DETAIL_POW_LOG_HPP_
#define INCLUDED_SLOWMATH_DETAIL_POW_LOG_HPP_


#include <type_traits>  // for integral_constant<>, make_unsigned<>, is_signed<>
#include <system_error> // for errc

#include <gsl/gsl-lite.hpp> // for Expects(), gsl_CPP17_OR_GREATER

#include <slowmath/detail/type_traits.hpp> // for min_v<>, max_v<>, common_integral_value_type<>, integral_value_type<>, result_t<>


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning( push )
# pragma warning( disable: 4702 ) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


namespace slowmath
{

namespace detail
{


    // Computes ⌊√v⌋.
    // This function is implemented with recursion, which is probably unsatisfactory for runtime performance, so we use it only internally, and only for compile-time computations.
template <typename V>
constexpr V sqrti(V v)
{
    Expects(v >= 0);

    V a = 0;
    if (v < 2) return v;
    a = sqrti<V>(v / 4) * 2; // a² ≤ v
    return v - a*a < 2*a + 1 // equivalent to `(a + 1)² > v` but without the possibility of overflow
        ? a
        : a + 1;
}

template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> square_unsigned(V v)
{
    using V0 = integral_value_type<V>;

    constexpr V0 m = detail::sqrti(max_v<V0>);

    if (v > m) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(v*v));
}
template <typename EH, typename V>
constexpr result_t<EH, integral_value_type<V>> square_signed(V v)
{
    using V0 = integral_value_type<V>;

    constexpr V0 m = detail::sqrti(max_v<V0>);

    if (v < -m || v > m) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(v*v));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename V>
constexpr int square_0(std::false_type /*isSigned*/, V v)
{
    return detail::square_unsigned<EH>(v);
}
template <typename EH, typename V>
constexpr int square_0(std::true_type /*isSigned*/, V v)
{
    return detail::square_signed<EH>(v);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename V>
constexpr int square(V v)
{
    using V0 = integral_value_type<V>;

#if gsl_CPP17_OR_GREATER
    if constexpr (std::is_signed_v<V0>) return detail::square_signed<EH>(v);
    else return detail::square_unsigned<EH>(v);
#else // gsl_CPP17_OR_GREATER
    return detail::square_0<EH>(std::is_signed<V0>{ }, v);
#endif // gsl_CPP17_OR_GREATER
}


template <typename U>
constexpr int bit_scan_reverse(U mask)
{
    int result = 0;
    while (mask >>= 1)
    {
        ++result;
    }
    return result;
}


    // Computes bᵉ for e ∊ ℕ₀.
template <typename EH, typename B, typename E>
constexpr result_t<EH, integral_value_type<B>> powi_unsigned(B b, E e)
{
    using V = integral_value_type<B>;
    using E0 = integral_value_type<E>;

        // Conventionally, `powi(0,0)` is 1.
    if (e == 0) return EH::make_result(V(e != 0 ? 0 : 1));
    if (b <= 1 || e == 1) return EH::make_result(V(b));

        // We assume `b > 1 && e > 1` henceforth.

    constexpr V mSq = detail::sqrti(max_v<V>);
    V mb = max_v<V> / b;

    V cb = 1;
    for (E0 bit = E0(1) << E0(detail::bit_scan_reverse(e)); bit > 0; bit /= 2)
    {
        if (cb > mSq) return EH::make_error(std::errc::value_too_large);
        cb *= cb;
        if ((e & bit) != 0)
        {
            if (cb > mb) return EH::make_error(std::errc::value_too_large);
            cb *= b;
        }
    }
    return EH::make_result(V(cb));
}
    // Computes bᵉ for e ∊ ℕ₀.
template <typename EH, typename B, typename E>
constexpr result_t<EH, integral_value_type<B>> powi_signed(B b, E e)
{
    using V = integral_value_type<B>;

        // In the special case of `b` assuming the smallest representable value, `signIn * b` would overflow,
        // so we need to handle it separately.
    if (b == min_v<V>)
    {
        if (e > 1) return EH::make_error(std::errc::value_too_large); // `powi()` would overflow for exponents greater than 1
        return EH::make_result(V(e == 0 ? V(1) : b));
    }

        // Factor out sign.
    V signIn = b >= 0
        ? 1
        : -1;
    V signOut = e % 2 == 0
        ? 1
        : signIn;

        // Compute `powi()` for unsigned positive number.
    using U = std::make_unsigned_t<V>;
    U absPow = detail::powi_unsigned<EH>(U(signIn * b), e);

        // Handle special case where result is smallest representable value.
    if (signOut == -1 && absPow == U(max_v<V>) + 1)
    {
        return EH::make_result(min_v<V>); // assuming two's complement
    }

        // Convert back to signed and prefix with sign.
    if (absPow > U(max_v<V>)) return EH::make_error(std::errc::value_too_large);
    return EH::make_result(V(signOut * V(absPow)));
}
#if !gsl_CPP17_OR_GREATER
template <typename EH, typename B, typename E>
constexpr result_t<EH, integral_value_type<B>> powi_0(std::false_type /*isSigned*/, B b, E e)
{
    return detail::powi_unsigned<EH>(b, e);
}
template <typename EH, typename B, typename E>
constexpr result_t<EH, integral_value_type<B>> powi_0(std::true_type /*isSigned*/, B b, E e)
{
    return detail::powi_signed<EH>(b, e);
}
#endif // !gsl_CPP17_OR_GREATER
template <typename EH, typename B, typename E>
constexpr result_t<EH, integral_value_type<B>> powi(B b, E e)
{
    using V = integral_value_type<B>;

        // Negative powers are not integral.
    Expects(e >= 0);

#if gsl_CPP17_OR_GREATER
    if constexpr (std::is_signed_v<V>) return detail::powi_signed<EH>(b, e);
    else return detail::powi_unsigned<EH>(b, e);
#else // gsl_CPP17_OR_GREATER
    return detail::powi_0<EH>(std::is_signed<V>{ }, b, e);
#endif // gsl_CPP17_OR_GREATER
}


    // Computes ⌊log x ÷ log b⌋ for x,b ∊ ℕ, x > 0, b > 1.
template <typename X, typename B>
constexpr common_integral_value_type<X, B> log_floori(X x, B b)
{
    using V = common_integral_value_type<X, B>;

    Expects(x > 0 && b > 1);

    V e = 0;
    V x0 = 1;
    constexpr V M = max_v<V>;
    V m = M / b;

        // Algorithm identical to `factorize_floori()`.
    for (;;)
    {
        if (x0 > m)
        {
            return e;
        }

        V x1 = x0 * b; // = bᵉ⁺¹
        if (x1 > x)
        {
            return e;
        }

        x0 = x1;
        ++e;
    }
}


    // Computes ⌈log x ÷ log b⌉ for x,b ∊ ℕ, x > 0, b > 1.
template <typename X, typename B>
constexpr common_integral_value_type<X, B> log_ceili(X x, B b)
{
    using V = common_integral_value_type<X, B>;

    Expects(x > 0 && b > 1);

    V e = 0;
    V x0 = 1;
    constexpr V M = max_v<V>;
    V m = M / b;

    while (x0 < x)
    {
        if (x0 > m)
        {
                // x₀ = bᵉ < x, otherwise we wouldn't be in the loop.
                // bᵉ > m implies bᵉ⁺¹ > M (cf. reasoning in factorize_floori()).
                // Because x ≤ M, ⌈log x ÷ log b⌉ = e + 1.
            return e + 1;
        }

        x0 *= b;
        ++e;
    }

    return e;
}


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
 #pragma warning( pop )
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_POW_LOG_HPP_
