
#ifndef INCLUDED_SLOWMATH_ARITHMETIC_HPP_
#define INCLUDED_SLOWMATH_ARITHMETIC_HPP_


#include <array>
#include <system_error> // for errc

#include <gsl-lite/gsl-lite.hpp> // for gsl_Expects(), gsl_CPP17_OR_GREATER, gsl_HAVE_EXCEPTIONS, gsl_NODISCARD

#include <slowmath/detail/type_traits.hpp> // for are_integral_arithmetic_types_v<>, have_same_signedness_v<>
#include <slowmath/detail/errors.hpp>      // for failfast_error_handler, try_error_handler, throw_error_handler

#include <slowmath/detail/arithmetic.hpp>  // for absi(), negate(), add(), subtract(), multiply(), divide(), modulo()
#include <slowmath/detail/bits.hpp>        // for shift_right(), shift_left()
#include <slowmath/detail/gcd-lcm.hpp>     // for gcd(), lcm()
#include <slowmath/detail/pow-log.hpp>     // for square(), powi(), log_floori(), log_ceili()
#include <slowmath/detail/round.hpp>       // for floori(), ceili(), ratio_floori(), ratio_ceili()
#include <slowmath/detail/factorize.hpp>   // for factorize_floori(), factorize_ceili()


namespace slowmath {


namespace gsl = ::gsl_lite;


template <typename V, typename E, int NumFactors>
struct factorization;
template <typename V, typename E>
struct factorization<V, E, 1>
{
    V remainder;
    E exponent1;

        // equivalence
    gsl_NODISCARD constexpr friend bool operator ==(factorization const& lhs, factorization const& rhs) noexcept
    {
        return lhs.remainder == rhs.remainder
            && lhs.exponent1 == rhs.exponent1;
    }
    gsl_NODISCARD constexpr friend bool operator !=(factorization const& lhs, factorization const& rhs) noexcept
    {
        return !(lhs == rhs);
    }
};
template <typename V, typename E>
struct factorization<V, E, 2>
{
    V remainder;
    E exponent1;
    E exponent2;

        // equivalence
    gsl_NODISCARD constexpr friend bool operator ==(factorization const& lhs, factorization const& rhs) noexcept
    {
        return lhs.remainder == rhs.remainder
            && lhs.exponent1 == rhs.exponent1
            && lhs.exponent2 == rhs.exponent2;
    }
    gsl_NODISCARD constexpr friend bool operator !=(factorization const& lhs, factorization const& rhs) noexcept
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


    //
    // Computes |v|.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
absi(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::absi<detail::ignore_error_handler>(v);
}

    //
    // Computes |v|.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
absi_failfast(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::absi<detail::failfast_error_handler>(v);
}

    //
    // Computes |v|.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<V>>
try_absi(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::absi<detail::try_error_handler>(v);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes |v|.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
absi_checked(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::absi<detail::throw_error_handler>(v);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes -v.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
negate_failfast(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::negate<detail::failfast_error_handler>(v);
}

    //
    // Computes -v.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<V>>
try_negate(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::negate<detail::try_error_handler>(v);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes -v.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
negate_checked(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::negate<detail::throw_error_handler>(v);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes a + b.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
add_failfast(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::add<detail::failfast_error_handler>(a, b);
}

    //
    // Computes a + b.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_add(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::add<detail::try_error_handler>(a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes a + b.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
add_checked(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::add<detail::throw_error_handler>(a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes a - b.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
subtract_failfast(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::subtract<detail::failfast_error_handler>(a, b);
}

    //
    // Computes a - b.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_subtract(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::subtract<detail::try_error_handler>(a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes a - b.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
subtract_checked(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::subtract<detail::throw_error_handler>(a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes a ∙ b.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
multiply_failfast(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::multiply<detail::failfast_error_handler>(a, b);
}

    //
    // Computes a ∙ b.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_multiply(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::multiply<detail::try_error_handler>(a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes a ∙ b.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
multiply_checked(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::multiply<detail::throw_error_handler>(a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes n ÷ d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
divide(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::divide<detail::ignore_error_handler>(n, d);
}

    //
    // Computes n ÷ d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
divide_failfast(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::divide<detail::failfast_error_handler>(n, d);
}

    //
    // Computes n ÷ d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<N, D>>
try_divide(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::divide<detail::try_error_handler>(n, d);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes n ÷ d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
divide_checked(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::divide<detail::throw_error_handler>(n, d);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes n mod d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
modulo(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::modulo<detail::ignore_error_handler>(n, d);
}

    //
    // Computes n mod d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
modulo_failfast(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::modulo<detail::failfast_error_handler>(n, d);
}

    //
    // Computes n mod d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<N, D>>
try_modulo(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::modulo<detail::try_error_handler>(n, d);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes n mod d for d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
modulo_checked(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(d != 0);

    return detail::modulo<detail::throw_error_handler>(n, d);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes a ∙ b.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
square(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "arguments must be integral types or std::integral_constant<> of integral types");

    return detail::square<detail::ignore_error_handler>(v);
}

    //
    // Computes a ∙ b.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
square_failfast(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "arguments must be integral types or std::integral_constant<> of integral types");

    return detail::square<detail::failfast_error_handler>(v);
}

    //
    // Computes a ∙ b.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<V>>
try_square(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::square<detail::try_error_handler>(v);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes a ∙ b.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename V>
gsl_NODISCARD constexpr detail::integral_value_type<V>
square_checked(V v)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<V>, "argument must be an integral type or an std::integral_constant<> of an integral type");

    return detail::square<detail::throw_error_handler>(v);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes x ∙ 2ˢ for x,s ∊ ℕ₀ (i.e. left-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::integral_value_type<X>
shift_left(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_left<detail::ignore_error_handler>(x, s);
}

    //
    // Computes x ∙ 2ˢ for x,s ∊ ℕ₀ (i.e. left-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::integral_value_type<X>
shift_left_failfast(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_left<detail::failfast_error_handler>(x, s);
}

    //
    // Computes x ∙ 2ˢ for x,s ∊ ℕ₀ (i.e. left-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<X>>
try_shift_left(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_left<detail::try_error_handler>(x, s);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes x ∙ 2ˢ for x,s ∊ ℕ₀ (i.e. left-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::integral_value_type<X>
shift_left_checked(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_left<detail::throw_error_handler>(x, s);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes ⌊x ÷ 2ˢ⌋ for x,s ∊ ℕ₀ (i.e. right-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::integral_value_type<X>
shift_right(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_right<detail::ignore_error_handler>(x, s);
}

    //
    // Computes ⌊x ÷ 2ˢ⌋ for x,s ∊ ℕ₀ (i.e. right-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::integral_value_type<X>
shift_right_failfast(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_right<detail::failfast_error_handler>(x, s);
}

    //
    // Computes ⌊x ÷ 2ˢ⌋ for x,s ∊ ℕ₀ (i.e. right-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<X>>
try_shift_right(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_right<detail::try_error_handler>(x, s);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes ⌊x ÷ 2ˢ⌋ for x,s ∊ ℕ₀ (i.e. right-shifts x by s bits).
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename S>
gsl_NODISCARD constexpr detail::integral_value_type<X>
shift_right_checked(X x, S s)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, S>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x >= 0 && s >= 0);

    return detail::shift_right<detail::throw_error_handler>(x, s);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes bᵉ for e ∊ ℕ₀.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename B, typename E>
gsl_NODISCARD constexpr detail::integral_value_type<B>
powi(B b, E e)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<B, E>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(e >= 0);

    return detail::powi<detail::ignore_error_handler>(b, e);
}

    //
    // Computes bᵉ for e ∊ ℕ₀.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename B, typename E>
gsl_NODISCARD constexpr detail::integral_value_type<B>
powi_failfast(B b, E e)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<B, E>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(e >= 0);

    return detail::powi<detail::failfast_error_handler>(b, e);
}

    //
    // Computes bᵉ for e ∊ ℕ₀.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename B, typename E>
gsl_NODISCARD constexpr arithmetic_result<detail::integral_value_type<B>>
try_powi(B b, E e)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<B, E>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(e >= 0);

    return detail::powi<detail::try_error_handler>(b, e);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes bᵉ for e ∊ ℕ₀.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename B, typename E>
gsl_NODISCARD constexpr detail::integral_value_type<B>
powi_checked(B b, E e)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<B, E>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(e >= 0);

    return detail::powi<detail::throw_error_handler>(b, e);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes ⌊x ÷ d⌋ ∙ d for x ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, D>
floori(X x, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    gsl_Expects(x >= 0 && d > 0);

    return detail::floori(x, d);
}


    //
    // Computes ⌈x ÷ d⌉ ∙ d for x ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, D>
ceili(X x, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    gsl_Expects(x >= 0 && d > 0);

    return detail::ceili<detail::ignore_error_handler>(x, d);
}

    //
    // Computes ⌈x ÷ d⌉ ∙ d for x ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, D>
ceili_failfast(X x, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    gsl_Expects(x >= 0 && d > 0);

    return detail::ceili<detail::failfast_error_handler>(x, d);
}

    //
    // Computes xᵉ for d ∊ ℕ₀.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<X, D>>
try_ceili(X x, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    gsl_Expects(x >= 0 && d > 0);

    return detail::ceili<detail::try_error_handler>(x, d);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Computes xᵉ for d ∊ ℕ₀.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename X, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<X, D>
ceili_checked(X x, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, D>, "argument types must have identical signedness");

    gsl_Expects(x >= 0 && d > 0);

    return detail::ceili<detail::throw_error_handler>(x, d);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes ⌊n ÷ d⌋ for n ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
ratio_floori(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(n >= 0 && d > 0);

    return detail::ratio_floori(n, d);
}


    //
    // Computes ⌈n ÷ d⌉ for n ∊ ℕ₀, d ∊ ℕ, d ≠ 0.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename N, typename D>
gsl_NODISCARD constexpr detail::common_integral_value_type<N, D>
ratio_ceili(N n, D d)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<N, D>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<N, D>, "argument types must have identical signedness");

    gsl_Expects(n >= 0 && d > 0);

    return detail::ratio_ceili(n, d);
}


    //
    // Computes ⌊log x ÷ log b⌋ for x,b ∊ ℕ, x > 0, b > 1.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename E, typename X, typename B>
gsl_NODISCARD constexpr E
log_floori(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, B>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x > 0 && b > 1);

    return detail::log_floori<E>(x, b);
}


    //
    // Computes ⌈log x ÷ log b⌉ for x,b ∊ ℕ, x > 0, b > 1.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename E, typename X, typename B>
gsl_NODISCARD constexpr E
log_ceili(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, B>, "arguments must be integral types or std::integral_constant<> of integral types");

    gsl_Expects(x > 0 && b > 1);

    return detail::log_ceili<E>(x, b);
}


    //
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, e) such that x = bᵉ + r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename E, typename X, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, B>, E, 1>
factorize_floori(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && b > 1);

    return detail::factorize_floori<E>(x, b);
}


    //
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, e) such that x = bᵉ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename E, typename X, typename B>
gsl_NODISCARD constexpr factorization<detail::integral_value_type<X>, E, 1>
factorize_ceili(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && b > 1);

    return detail::factorize_ceili<detail::ignore_error_handler, E>(x, b);
}

    //
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, e) such that x = bᵉ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename E, typename X, typename B>
gsl_NODISCARD constexpr factorization<detail::integral_value_type<X>, E, 1>
factorize_ceili_failfast(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && b > 1);

    return detail::factorize_ceili<detail::failfast_error_handler, E>(x, b);
}

    //
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, e) such that x = bᵉ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename E, typename X, typename B>
gsl_NODISCARD constexpr arithmetic_result<factorization<detail::common_integral_value_type<X, B>, E, 1>>
try_factorize_ceili(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && b > 1);

    return detail::factorize_ceili<detail::try_error_handler, E>(x, b);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Given x,b ∊ ℕ, x > 0, b > 1, returns (r, e) such that x = bᵉ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename E, typename X, typename B>
gsl_NODISCARD constexpr factorization<detail::integral_value_type<X>, E, 1>
factorize_ceili_checked(X x, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && b > 1);

    return detail::factorize_ceili<detail::throw_error_handler, E>(x, b);
}
#endif // gsl_HAVE_EXCEPTIONS


    //
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r, i, j) such that x = aⁱ ∙ bʲ + r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename E, typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, A, B>, E, 2>
factorize_floori(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_floori<detail::integral_value_type<X>, E>(x, a, b);
}


    //
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r, i, j) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`.
    //
template <typename E, typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, A, B>, E, 2>
factorize_ceili(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_ceili<detail::ignore_error_handler, E>(x, a, b);
}

    //
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r, i, j) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Uses `assert()` to check that no overflow occurs.
    //
template <typename E, typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, A, B>, E, 2>
factorize_ceili_failfast(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_ceili<detail::failfast_error_handler, E>(x, a, b);
}

    //
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r,{ i, j }) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename E, typename X, typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<factorization<detail::common_integral_value_type<X, A, B>, E, 2>>
try_factorize_ceili(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_ceili<detail::try_error_handler, E>(x, a, b);
}

#if gsl_HAVE_EXCEPTIONS
    //
    // Given x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b, returns (r,{ i, j }) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal.
    //ᅟ
    // Enforces preconditions with `gsl_Expects()`. Throws `std::system_error` upon overflow.
    //
template <typename E, typename X, typename A, typename B>
gsl_NODISCARD constexpr factorization<detail::common_integral_value_type<X, A, B>, E, 2>
factorize_ceili_checked(X x, A a, B b)
{
    static_assert(detail::are_integral_arithmetic_types_v<E>, "result type must be an integral type");
    static_assert(detail::are_value_types_integral_arithmetic_types_v<X, A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<X, A, B>, "argument types must have identical signedness");

    gsl_Expects(x > 0 && a > 1 && b > 1 && a != b);

    return detail::factorize_ceili<detail::throw_error_handler, E>(x, a, b);
}
#endif // gsl_HAVE_EXCEPTIONS


#if gsl_CPP17_OR_GREATER
    //
    // Computes the greatest common divisor of a and b.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
gcd_failfast(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::gcd<detail::failfast_error_handler>(a, b);
}

    //
    // Computes the greatest common divisor of a and b.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_gcd(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::gcd<detail::try_error_handler>(a, b);
}

# if gsl_HAVE_EXCEPTIONS
    //
    // Computes the greatest common divisor of a and b.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
gcd_checked(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::gcd<detail::throw_error_handler>(a, b);
}
# endif // gsl_HAVE_EXCEPTIONS


    //
    // Computes the least common multiple of a and b.
    //ᅟ
    // Uses `assert()` to check that no overflow occurs.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
lcm_failfast(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::lcm<detail::failfast_error_handler>(a, b);
}

    //
    // Computes the least common multiple of a and b.
    //ᅟ
    // Returns error code `std::errc::value_too_large` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr arithmetic_result<detail::common_integral_value_type<A, B>>
try_lcm(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::lcm<detail::try_error_handler>(a, b);
}

# if gsl_HAVE_EXCEPTIONS
    //
    // Computes the least common multiple of a and b.
    //ᅟ
    // Throws `std::system_error` upon overflow.
    //
template <typename A, typename B>
gsl_NODISCARD constexpr detail::common_integral_value_type<A, B>
lcm_checked(A a, B b)
{
    static_assert(detail::are_value_types_integral_arithmetic_types_v<A, B>, "arguments must be integral types or std::integral_constant<> of integral types");
    static_assert(detail::have_same_signedness_v<A, B>, "argument types must have identical signedness");

    return detail::lcm<detail::throw_error_handler>(a, b);
}
# endif // gsl_HAVE_EXCEPTIONS
#endif // gsl_CPP17_OR_GREATER


} // namespace slowmath


#endif // INCLUDED_SLOWMATH_ARITHMETIC_HPP_
