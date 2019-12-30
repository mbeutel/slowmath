
#ifndef INCLUDED_SLOWMATH_DETAIL_TYPE_TRAITS_HPP_
#define INCLUDED_SLOWMATH_DETAIL_TYPE_TRAITS_HPP_


#include <limits>
#include <cstdint>      // for [u]int(8|16|32|64)_t
#include <type_traits>  // for integral_constant<>, make_unsigned<>, is_signed<>, is_integral<>, is_same<>, common_type<>

#include <gsl-lite/gsl-lite.hpp> // for conjunction<>


namespace slowmath
{


namespace gsl = ::gsl_lite;


namespace detail
{


template <typename V> struct integral_value_type_ { using type = V; };
template <typename V, V Value> struct integral_value_type_<std::integral_constant<V, Value>> { using type = V; };
template <typename V> using integral_value_type = typename integral_value_type_<V>::type;

template <typename... Vs> using common_integral_value_type = std::common_type_t<typename integral_value_type_<Vs>::type...>;

template <typename V> struct is_nonbool_integral : std::is_integral<V> { };
template <> struct is_nonbool_integral<bool> : std::false_type { };

template <typename V> struct is_value_type_nonbool_integral : is_nonbool_integral<V> { };
template <typename V, V Value> struct is_value_type_nonbool_integral<std::integral_constant<V, Value>> : is_nonbool_integral<V> { };

template <typename... Vs> constexpr bool are_integral_arithmetic_types_v = gsl::conjunction<is_nonbool_integral<Vs>...>::value;

template <typename... Vs> constexpr bool are_value_types_integral_arithmetic_types_v = gsl::conjunction<is_value_type_nonbool_integral<Vs>...>::value;

template <bool Signed, typename... Vs> using have_same_signedness_0 = gsl::conjunction<std::integral_constant<bool, std::is_signed<integral_value_type<Vs>>::value == Signed>...>;
template <typename... Vs> struct have_same_signedness;
template <typename V0, typename... Vs> struct have_same_signedness<V0, Vs...> : have_same_signedness_0<std::is_signed<integral_value_type<V0>>::value, Vs...> { };
template <> struct have_same_signedness<> : std::true_type { };
template <typename... Vs> constexpr bool have_same_signedness_v = have_same_signedness<Vs...>::value;

template <typename V> struct wider_type_;
template <typename V> using wider_type = typename wider_type_<V>::type;
template <> struct wider_type_<std::int8_t> { using type = std::int32_t; };
template <> struct wider_type_<std::int16_t> { using type = std::int32_t; };
template <> struct wider_type_<std::int32_t> { using type = std::int64_t; };
template <> struct wider_type_<std::uint8_t> { using type = std::uint32_t; };
template <> struct wider_type_<std::uint16_t> { using type = std::uint32_t; };
template <> struct wider_type_<std::uint32_t> { using type = std::uint64_t; };
template <typename V> constexpr bool has_wider_type_v = sizeof(V) <= sizeof(std::uint32_t);
template <typename V> struct has_wider_type : std::integral_constant<bool, has_wider_type_v<V>> { };

template <typename EH, typename V> using result_t = typename EH::template result<V>;

template <typename T> constexpr T min_v = std::numeric_limits<T>::min();
template <typename T> constexpr T max_v = std::numeric_limits<T>::max();


} // namespace detail

} // namespace slowmath


#endif // INCLUDED_SLOWMATH_DETAIL_TYPE_TRAITS_HPP_
