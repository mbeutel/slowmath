
#ifndef INCLUDED_SLOWMATH_FENV_HPP_
#define INCLUDED_SLOWMATH_FENV_HPP_


#include <gsl-lite/gsl-lite.hpp>  // for gsl_Expects(), gsl_NODISCARD

#include <slowmath/detail/errors.hpp>
#include <slowmath/detail/fenv.hpp>


namespace slowmath {


namespace gsl = ::gsl_lite;


    //
    // Sets hardware exception traps for the floating-point exceptions specified by the given mask value.
    //ᅟ
    // The admissible mask values are defined as `FE_*` in standard header <cfenv>.
    // If an exception flag bit is set, the corresponding exception will be trapped; if the bit is clear, the exception will be
    // masked.
    //
inline void
fe_set_trapping_exceptions(int excepts)
{
    gsl_Expects((excepts & ~FE_ALL_EXCEPT) == 0);

#if defined(_WIN32)
    unsigned oldFlags;
    unsigned flags = static_cast<unsigned>(FE_ALL_EXCEPT & ~excepts); // convert flags
    detail::posix_assert(_controlfp_s(&oldFlags, flags, _MCW_EM) == 0);
#elif defined(__linux__) || defined(SLOWMATH_DETAIL_APPLE_INTEL)
# ifdef __APPLE__
    using detail::fegetexcept;
    using detail::feenableexcept;
    using detail::fedisableexcept;
# endif // __APPLE__
    int flags = fegetexcept();
    if (flags == -1) throw std::runtime_error("fegetexcept() call failed");
    int exceptsToEnable = excepts & ~(flags & FE_ALL_EXCEPT);
    int exceptsToDisable = ~excepts & (flags & FE_ALL_EXCEPT);
    if (exceptsToEnable != 0)
    {
        int result = feenableexcept(exceptsToEnable);
        if (result == -1) throw std::runtime_error("feenableexcept() call failed");
    }
    if (exceptsToDisable != 0)
    {
        int result = fedisableexcept(exceptsToDisable);
        if (result == -1) throw std::runtime_error("fedisableexcept() call failed");
    }
#else
 #error Unsupported operating system.
#endif
}

    //
    // Returns the bitmask of all floating-point exceptions for which trapping is currently enabled.
    //ᅟ
    // The admissible mask values are defined as `FE_*` in standard header <cfenv>.
    //
gsl_NODISCARD inline int
fe_get_trapping_exceptions(void)
{
#if defined(_WIN32)
    unsigned flags;
    detail::posix_assert(_controlfp_s(&flags, 0, 0) == 0);
    return FE_ALL_EXCEPT & ~static_cast<int>(flags); // convert flags
#elif defined(__linux__) || defined(SLOWMATH_DETAIL_APPLE_INTEL)
# ifdef __APPLE__
    using detail::fegetexcept;
# endif // __APPLE__
    int flags = fegetexcept();
    if (flags == -1) throw std::runtime_error("fegetexcept() call failed");
    return flags & FE_ALL_EXCEPT;
#else
 #error Unsupported operating system.
#endif
}


} // namespace slowmath


#endif // INCLUDED_SLOWMATH_FENV_HPP_
