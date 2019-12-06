
#ifndef INCLUDED_SLOWMATH_DETAIL_POW_LOG_HPP_
#define INCLUDED_SLOWMATH_DETAIL_POW_LOG_HPP_


#include <type_traits>  // for integral_constant<>, make_unsigned<>, is_signed<>
#include <system_error> // for errc

#include <gsl/gsl-lite.hpp> // for Expects(), gsl_CPP17_OR_GREATER

#include <slowmath/detail/type_traits.hpp> // for min_v<>, max_v<>, common_integral_value_type<>, integral_value_type<>, result_t<>


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning( push )
# pragma warning( disable: 4146 ) // unary minus operator applied to unsigned type, result still unsigned
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

    // Computes v².
template <typename EH, typename V>
constexpr int square(V v)
{
    using V0 = integral_value_type<V>;

    constexpr V0 m = detail::sqrti(max_v<V0>);
    if (v > m || (std::is_signed<V0>::value && v < -m)) return EH::make_error(std::errc::value_too_large);

    return EH::make_result(V(v*v));
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


    // Computes bᵉ for b, e ∊ ℕ₀.
template <typename EH, typename B, typename E>
constexpr result_t<EH, integral_value_type<B>> powi_0(B b, E e)
{
    using V = integral_value_type<B>;
    using E0 = integral_value_type<E>;

    constexpr V mSq = detail::sqrti(max_v<V>);
    V mb = max_v<V> / b;

    V cb = 1;
    for (E0 bit = E0(1) << E0(detail::bit_scan_reverse(e)); bit > 0; bit >>= 1)
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
template <typename EH, typename B, typename E>
constexpr result_t<EH, integral_value_type<B>> powi(B b, E e)
{
    using V = integral_value_type<B>;
    using U = std::make_unsigned_t<V>;

        // Case needs to be handled separately to avoid division by zero in `powi_0()`.
    if (b == 0) return EH::make_result(V(e == 0 ? 1 : 0));

    if (b < 0)
    {
            // Compute `powi()` for unsigned positive number. Here we exploit that `-U(min_v<V>) == U(min_v<V>)`.
        U uresult = powi_0<EH>(U(-U(b)), e);
        bool negate = e % 2 != 0;

            // Check for overflow (note the slight differente for positive vs. negative results).
        if (uresult > U(min_v<V>) || (uresult == U(min_v<V>) && !negate)) return EH::make_error(std::errc::value_too_large);

        return EH::make_result(negate ? V(-uresult) : V(uresult));
    }
    return detail::powi_0<EH>(b, e);
}


    // Computes ⌊log x ÷ log b⌋ for x,b ∊ ℕ, x > 0, b > 1.
template <typename E, typename X, typename B>
constexpr E log_floori(X x, B b)
{
    using V = common_integral_value_type<X, B>;

    E e = 0;
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
template <typename E, typename X, typename B>
constexpr E log_ceili(X x, B b)
{
    using V = common_integral_value_type<X, B>;

    E e = 0;
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
