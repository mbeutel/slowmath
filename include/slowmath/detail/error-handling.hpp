
#ifndef INCLUDED_SLOWMATH_DETAIL_ERROR_HANDLING_HPP_
#define INCLUDED_SLOWMATH_DETAIL_ERROR_HANDLING_HPP_


#include <exception>    // for terminate()
#include <type_traits>  // for integral_constant<>
#include <system_error> // for errc, system_error, error_code

#include <gsl/gsl-lite.hpp> // for Expects()


#if defined(_MSC_VER) && !defined(__clang__)
# pragma warning( push )
# pragma warning( disable: 4702 ) // unreachable code
#endif // defined(_MSC_VER) && !defined(__clang__)


#if defined(_MSC_VER)
# define SLOWMATH_DETAIL_FORCEINLINE  __forceinline
#elif defined(__INTEL_COMPILER)
# define SLOWMATH_DETAIL_FORCEINLINE  __forceinline // TODO: ??
#elif defined(__GNUC__)
# define SLOWMATH_DETAIL_FORCEINLINE  __attribute__((always_inline)) inline
#else
# define SLOWMATH_DETAIL_FORCEINLINE  inline
#endif


namespace slowmath
{


template <typename T>
struct arithmetic_result;


namespace detail
{


struct unreachable_wildcard_t
{
    template <typename T>
    [[noreturn]] operator T(void) const noexcept
    {
        std::terminate(); // unreachable
    }
};

struct errc_wildcard_t
{
    std::errc ec;

    template <typename T>
    constexpr operator arithmetic_result<T>(void) const noexcept
    {
        return { { }, ec };
    }
};

struct assert_error_handler
{
    template <typename T> using result = T;
    
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE T make_result(T value) noexcept
    {
        return value;
    }
    static inline unreachable_wildcard_t make_error(std::errc)
    {
        Expects(false);
    }
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE T get_value(T result) noexcept
    {
        return result;
    }
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE std::false_type is_error(T) noexcept
    {
        return { };
    }
    template <typename T>
    static SLOWMATH_DETAIL_FORCEINLINE unreachable_wildcard_t passthrough_error(T)
    {
        Expects(false);
    }
};

struct try_error_handler
{
    template <typename T> using result = arithmetic_result<T>;
    
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE arithmetic_result<T> make_result(T value) noexcept
    {
        return { value, std::errc{ } };
    }
    static constexpr errc_wildcard_t make_error(std::errc ec) noexcept
    {
        return { ec };
    }
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE T get_value(arithmetic_result<T> const& result) noexcept
    {
        return result.value;
    }
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE bool is_error(arithmetic_result<T> const& result) noexcept
    {
        return result.ec != std::errc{ };
    }
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE errc_wildcard_t passthrough_error(arithmetic_result<T> const& result) noexcept
    {
        return { result.ec };
    }
};

struct throw_error_handler
{
    template <typename T> using result = T;
    
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE T make_result(T value) noexcept
    {
        return value;
    }
    [[noreturn]] static inline unreachable_wildcard_t make_error(std::errc ec)
    {
        throw std::system_error(std::make_error_code(ec));
    }
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE T get_value(T result) noexcept
    {
        return result;
    }
    template <typename T>
    static constexpr SLOWMATH_DETAIL_FORCEINLINE std::false_type is_error(T) noexcept
    {
        return { };
    }
    template <typename T>
    static SLOWMATH_DETAIL_FORCEINLINE unreachable_wildcard_t passthrough_error(T) noexcept
    {
        std::terminate();
    }
};


} // namespace detail

} // namespace slowmath


#if defined(_MSC_VER) && !defined(__clang__)
 #pragma warning( pop )
#endif // defined(_MSC_VER) && !defined(__clang__)


#endif // INCLUDED_SLOWMATH_DETAIL_ERROR_HANDLING_HPP_