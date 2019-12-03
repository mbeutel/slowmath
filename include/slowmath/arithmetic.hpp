
#ifndef INCLUDED_SLOWMATH_ARITHMETIC_HPP_
#define INCLUDED_SLOWMATH_ARITHMETIC_HPP_


#include <array>
#include <system_error> // for errc

#include <gsl/gsl-lite.hpp> // for Expects(), gsl_CPP17_OR_GREATER, gsl_HAVE_EXCEPTIONS, gsl_NODISCARD, gsl_constexpr20

#include <slowmath/detail/type_traits.hpp>    // for are_integral_arithmetic_types_v<>, have_same_signedness_v<>
#include <slowmath/detail/error-handling.hpp> // for assert_error_handler, try_error_handler, throw_error_handler

#include <slowmath/detail/arithmetic.hpp>     // for absi(), negate(), add(), subtract(), multiply(), divide(), modulo()
#include <slowmath/detail/bits.hpp>           // for shift_right(), shift_left()
#include <slowmath/detail/gcd-lcm.hpp>        // for gcd(), lcm()
#include <slowmath/detail/pow-log.hpp>        // for square(), powi(), log_floori(), log_ceili()
#include <slowmath/detail/round.hpp>          // for floori(), ceili(), ratio_floori(), ratio_ceili()
#include <slowmath/detail/factorize.hpp>      // for factorize_floori(), factorize_ceili()
#include <slowmath/detail/cast.hpp>           // for integral_cast<>()


namespace slowmath
{


template <typename V, int NumFactors>
struct factorization
{
    V remainder;
    std::array<V, NumFactors> factorExponents;

        // equivalence
    gsl_NODISCARD gsl_constexpr20 friend bool operator ==(factorization const& lhs, factorization const& rhs) noexcept
    {
        return lhs.remainder == rhs.remainder
            && lhs.factorExponents == rhs.factorExponents;
    }
    gsl_NODISCARD gsl_constexpr20 friend bool operator !=(factorization const& lhs, factorization const& rhs) noexcept
    {
        return !(lhs == rhs);
    }
};


template <typename T>
struct arithmetic_result
{
    T value;
    std::errc ec;
};


    //ᅟ
    // Computes |v|.
    // Uses `Expects()` to raise error upon underflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
absi(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::absi<detail::assert_error_handler>(v);
}

    //ᅟ
    // Computes |v|.
    // Returns error code `std::errc::value_too_large` upon underflow.
    //
template <typename V>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<V>>
try_absi(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::absi<detail::try_error_handler>(v);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes |v|.
    // Throws `std::system_error` upon underflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
absi_or_throw(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::absi<detail::throw_error_handler>(v);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes -v.
    // Uses `Expects()` to raise error upon underflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
negate(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::negate<detail::assert_error_handler>(v);
}

    //ᅟ
    // Computes -v.
    // Returns error code `std::errc::value_too_large` upon underflow.
    //
template <typename V>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<V>>
try_negate(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::negate<detail::try_error_handler>(v);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes -v.
    // Throws `std::system_error` upon underflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
negate_or_throw(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::negate<detail::throw_error_handler>(v);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes a + b.
    // Uses `Expects()` to raise error upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
add(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::add<detail::assert_error_handler>(a, b);
}

    //ᅟ
    // Computes a + b.
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_add(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::add<detail::try_error_handler>(a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes a + b.
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
add_or_throw(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::add<detail::throw_error_handler>(a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes a - b.
    // Uses `Expects()` to raise error upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
subtract(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::subtract<detail::assert_error_handler>(a, b);
}

    //ᅟ
    // Computes a - b.
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_subtract(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::subtract<detail::try_error_handler>(a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes a - b.
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
subtract_or_throw(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::subtract<detail::throw_error_handler>(a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes a ∙ b.
    // Uses `Expects()` to raise error upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
multiply(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::multiply<detail::assert_error_handler>(a, b);
}

    //ᅟ
    // Computes a ∙ b.
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_multiply(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::multiply<detail::try_error_handler>(a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes a ∙ b.
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
multiply_or_throw(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::multiply<detail::throw_error_handler>(a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes n ÷ d for d ≠ 0.
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
divide(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(d != 0);

    return detail::divide<detail::assert_error_handler>(n, d);
}

    //ᅟ
    // Computes n ÷ d for d ≠ 0.
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<N, D>>
try_divide(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(d != 0);

    return detail::divide<detail::try_error_handler>(n, d);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes n ÷ d for d ≠ 0.
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
divide_or_throw(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(d != 0);

    return detail::divide<detail::throw_error_handler>(n, d);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes n mod d for d ≠ 0.
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
modulo(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(d != 0);

    return detail::modulo<detail::assert_error_handler>(n, d);
}

    //ᅟ
    // Computes n mod d for d ≠ 0.
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<N, D>>
try_modulo(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(d != 0);

    return detail::modulo<detail::try_error_handler>(n, d);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes n mod d for d ≠ 0.
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
modulo_or_throw(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(d != 0);

    return detail::modulo<detail::throw_error_handler>(n, d);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes a ∙ b.
    // Uses `Expects()` to raise error upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
square(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "arguments must be convertible to an integral type");

    return detail::square<detail::assert_error_handler>(v);
}

    //ᅟ
    // Computes a ∙ b.
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<V>>
try_square(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::square<detail::try_error_handler>(v, v);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes a ∙ b.
    // Throws `std::system_error` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
square_or_throw(V v)
{
    static_assert(detail::are_integral_arithmetic_types_v<V>, "argument must be convertible to an integral type");

    return detail::square<detail::throw_error_handler>(v, v);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes x ∙ 2ˢ for x,s ∊ ℕ₀ (i.e. left-shifts x by s bits).
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, S>
shift_left(X x, S s)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, S>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, S>, "argument types must have identical signedness");

    Expects(x >= 0 && s >= 0);

    return detail::shift_left<detail::assert_error_handler>(x, s);
}

    //ᅟ
    // Computes x ∙ 2ˢ for x,s ∊ ℕ₀ (i.e. left-shifts x by s bits).
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<X, S>>
try_shift_left(X x, S s)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, S>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, S>, "argument types must have identical signedness");

    Expects(x >= 0 && s >= 0);

    return detail::shift_left<detail::try_error_handler>(x, s);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes x ∙ 2ˢ for x,s ∊ ℕ₀ (i.e. left-shifts x by s bits).
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, S>
shift_left_or_throw(X x, S s)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, S>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, S>, "argument types must have identical signedness");

    Expects(x >= 0 && s >= 0);

    return detail::shift_left<detail::throw_error_handler>(x, s);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes ⌊x ÷ 2ˢ⌋ for x,s ∊ ℕ₀ (i.e. right-shifts x by s bits).
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, S>
shift_right(X x, S s)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, S>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, S>, "argument types must have identical signedness");

    Expects(x >= 0 && s >= 0);

    return detail::shift_right<detail::assert_error_handler>(x, s);
}

    //ᅟ
    // Computes ⌊x ÷ 2ˢ⌋ for x,s ∊ ℕ₀ (i.e. right-shifts x by s bits).
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<X, S>>
try_shift_right(X x, S s)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, S>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, S>, "argument types must have identical signedness");

    Expects(x >= 0 && s >= 0);

    return detail::shift_right<detail::try_error_handler>(x, s);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes ⌊x ÷ 2ˢ⌋ for x,s ∊ ℕ₀ (i.e. right-shifts x by s bits).
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, S>
shift_right_or_throw(X x, S s)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, S>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, S>, "argument types must have identical signedness");

    Expects(x >= 0 && s >= 0);

    return detail::shift_right<detail::throw_error_handler>(x, s);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes bᵉ for e ∊ ℕ₀.
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename B, typename E>
gsl_NODISCARD constexpr detail::integral_value_type<B>
powi(B b, E e)
{
    static_assert(detail::are_integral_arithmetic_types_v<B, E>, "arguments must be convertible to an integral type");

    Expects(e >= 0);

    return detail::powi<detail::assert_error_handler>(b, e);
}

    //ᅟ
    // Computes bᵉ for e ∊ ℕ₀.
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename B, typename E>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<B>>
try_powi(B b, E e)
{
    static_assert(detail::are_integral_arithmetic_types_v<B, E>, "arguments must be convertible to an integral type");

    Expects(e >= 0);

    return detail::powi<detail::try_error_handler>(b, e);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes bᵉ for e ∊ ℕ₀.
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename B, typename E>
gsl_NODISCARD constexpr detail::integral_value_type<B>
powi_or_throw(B b, E e)
{
    static_assert(detail::are_integral_arithmetic_types_v<B, E>, "arguments must be convertible to an integral type");

    Expects(e >= 0);

    return detail::powi<detail::throw_error_handler>(b, e);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes ⌊x ÷ d⌋ ∙ d for x ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    // Enforces preconditions with `Expects()`.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, D>
floori(X x, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    Expects(x >= 0 && d > 0);

    return detail::floori(x, d);
}


    //ᅟ
    // Computes ⌈x ÷ d⌉ ∙ d for x ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, D>
ceili(X x, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    Expects(x >= 0 && d > 0);

    return detail::ceili<detail::assert_error_handler>(x, d);
}

    //ᅟ
    // Computes xᵉ for d ∊ ℕ₀.
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<X, D>>
try_ceili(X x, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    Expects(x >= 0 && d > 0);

    return detail::ceili<detail::try_error_handler>(x, d);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes xᵉ for d ∊ ℕ₀.
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, D>
ceili_or_throw(X x, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    Expects(x >= 0 && d > 0);

    return detail::ceili<detail::throw_error_handler>(x, d);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes ⌊n ÷ d⌋ for n ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    // Enforces preconditions with `Expects()`.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
ratio_floori(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(n >= 0 && d > 0);

    return detail::ratio_floori(n, d);
}


    //ᅟ
    // Computes ⌈n ÷ d⌉ for n ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    // Enforces preconditions with `Expects()`.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
ratio_ceili(N n, D d)
{
    static_assert(detail::are_integral_arithmetic_types_v<N, D>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    Expects(n >= 0 && d > 0);

    return detail::ratio_ceili(n, d);
}


    //ᅟ
    // Computes ⌊log x ÷ log b⌋ for x,b ∊ ℕ, x > 0, b > 1.
    // Enforces preconditions with `Expects()`.
    //
template <typename X, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, B>
log_floori(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    Expects(x > 0 && b > 1);

    return detail::log_floori(x, b);
}


    //ᅟ
    // Computes ⌈log x ÷ log b⌉ for x,b ∊ ℕ, x > 0, b > 1.
    // Enforces preconditions with `Expects()`.
    //
template <typename X, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, B>
log_ceili(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    Expects(x > 0 && b > 1);

    return detail::log_ceili(x, b);
}


    //ᅟ
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, { e }) such that x = bᵉ + r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`.
    //
template <typename X, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, B>, 1>
factorize_floori(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    Expects(x > 0 && b > 1);

    return detail::factorize_floori(x, b);
}


    //ᅟ
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, { e }) such that x = bᵉ - r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename X, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, B>, 1>
factorize_ceili(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    Expects(x > 0 && b > 1);

    return detail::factorize_ceili<detail::assert_error_handler>(x, b);
}

    //ᅟ
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, { e }) such that x = bᵉ - r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename B>
gsl_NODISCARD constexpr factorization<arithmetic_result<detail::common_integral_value_type<X, B>>, 1>
try_factorize_ceili(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    Expects(x > 0 && b > 1);

    return detail::factorize_ceili<detail::try_error_handler>(x, b);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, { e }) such that x = bᵉ - r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, B>, 1>
factorize_ceili_or_throw(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    Expects(x > 0 && b > 1);

    return detail::factorize_ceili<detail::throw_error_handler>(x, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r, { i, j }) such that x = aⁱ ∙ bʲ + r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`.
    //
template <typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, A, B>, 2>
factorize_floori(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_floori(x, a, b);
}


    //ᅟ
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r, { i, j }) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`. Uses `Expects()` to raise error upon overflow.
    //
template <typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, A, B>, 2>
factorize_ceili(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_ceili<detail::assert_error_handler>(x, a, b);
}

    //ᅟ
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r,{ i, j }) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<arithmetic_result<detail::common_integral_value_type<X, A, B>>, 2>
try_factorize_ceili(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_ceili<detail::try_error_handler>(x, a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r,{ i, j }) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
    // Enforces preconditions with `Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, A, B>, 2>
factorize_ceili_or_throw(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<X, A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_ceili<detail::throw_error_handler>(x, a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


#if gsl_CPP17_OR_GREATER
    //ᅟ
    // Computes the greatest common divisor of a and b.
    // Uses `Expects()` to raise error upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
gcd(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::gcd<detail::assert_error_handler>(a, b);
}

    //ᅟ
    // Computes the greatest common divisor of a and b.
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_gcd(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::gcd<detail::try_error_handler>(a, b);
}

# if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes the greatest common divisor of a and b.
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
gcd_or_throw(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::gcd<detail::throw_error_handler>(a, b);
}
# endif // gsl_HAVE_EXCEPTIONS


    //ᅟ
    // Computes the least common multiple of a and b.
    // Uses `Expects()` to raise error upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
lcm(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::lcm<detail::assert_error_handler>(a, b);
}

    //ᅟ
    // Computes the least common multiple of a and b.
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_lcm(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::lcm<detail::try_error_handler>(a, b);
}

# if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Computes the least common multiple of a and b.
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
lcm_or_throw(A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<A, B>, "arguments must be convertible to an integral type");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::lcm<detail::throw_error_handler>(a, b);
}
# endif // gsl_HAVE_EXCEPTIONS
#endif // gsl_CPP17_OR_GREATER


    //ᅟ
    // Casts the given integer value to `DstT`.
    // Uses `Expects()` to raise error if the value cannot be represented in the target type.
    //
template <typename DstT, typename SrcT>
gsl_NODISCARD constexpr DstT
integral_cast(SrcT src)
{
    static_assert(detail::are_integral_arithmetic_types_v<DstT, SrcT>, "arguments must be convertible to an integral type");

    return detail::integral_cast<detail::assert_error_handler, DstT>(src);
}

    //ᅟ
    // Casts the given integer value to `DstT`.
    // Returns error code `std::errc::value_too_large` if the value cannot be represented in the target type.
    //
template <typename DstT, typename SrcT>
gsl_NODISCARD constexpr arithmetic_result<DstT>
try_integral_cast(SrcT src)
{
    static_assert(detail::are_integral_arithmetic_types_v<DstT, SrcT>, "arguments must be convertible to an integral type");

    return detail::integral_cast<detail::try_error_handler, DstT>(src);
}

#if gsl_HAVE_EXCEPTIONS
    //ᅟ
    // Casts the given integer value to `DstT`.
    // Throws `std::system_error` if the value cannot be represented in the target type.
    //
template <typename DstT, typename SrcT>
gsl_NODISCARD constexpr DstT
integral_cast_or_throw(SrcT src)
{
    static_assert(detail::are_integral_arithmetic_types_v<DstT, SrcT>, "arguments must be convertible to an integral type");

    return detail::integral_cast<detail::throw_error_handler, DstT>(src);
}
#endif // gsl_HAVE_EXCEPTIONS


} // namespace slowmath


#endif // INCLUDED_SLOWMATH_ARITHMETIC_HPP_
