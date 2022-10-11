
#ifndef INCLUDED_SLOWMATH_DETAIL_FENV_HPP_
#define INCLUDED_SLOWMATH_DETAIL_FENV_HPP_


#include <cfenv>  // for FE_*

#if defined(__APPLE__) && defined(__MACH__) && (defined(__i386__) || defined(__x86_64__))
# define SLOWMATH_DETAIL_APPLE_INTEL
#endif // defined(__APPLE__) && defined(__MACH__) && (defined(__i386__) || defined(__x86_64__))

#if defined(_WIN32)
# include <float.h>
#elif defined(__linux__) || defined(SLOWMATH_DETAIL_APPLE_INTEL)
# include <fenv.h>
#else
# error Unsupported operating system.
#endif

#include <slowmath/detail/errors.hpp>


namespace slowmath {

namespace detail {


#if defined(_WIN32)
    // We assume that the flags for C++11 floating-point settings and for `_controlfp_s()` have identical values.
static_assert(FE_DIVBYZERO == _EM_ZERODIVIDE
           && FE_INEXACT == _EM_INEXACT
           && FE_INVALID == _EM_INVALID
           && FE_OVERFLOW == _EM_OVERFLOW
           && FE_UNDERFLOW == _EM_UNDERFLOW, "static assertion failed");
#endif // defined(_WIN32)

#ifdef SLOWMATH_DETAIL_APPLE_INTEL
// borrowed from http://www-personal.umich.edu/~williams/archive/computation/fe-handling-example.c
static int
fegetexcept(void)
{
    fenv_t fenv;
    if (fegetenv(&fenv))
    {
        return -1;
    }
    return ~fenv.__control & FE_ALL_EXCEPT; // Note that the original source code in fe-handling-example.c has a bug here.
}
static int
feenableexcept(int excepts)
{
    fenv_t fenv;
    int new_excepts = excepts & FE_ALL_EXCEPT;
    // previous masks
    int old_excepts;

    if (fegetenv(&fenv))
    {
        return -1;
    }
    old_excepts = ~fenv.__control & FE_ALL_EXCEPT;

    // unmask
    fenv.__control &= ~new_excepts;
    fenv.__mxcsr   &= ~(new_excepts << 7);

    return fesetenv(&fenv) ? -1 : old_excepts;
}
static int
fedisableexcept(int excepts)
{
    fenv_t fenv;
    int new_excepts = excepts & FE_ALL_EXCEPT;
    // all previous masks
    int old_excepts;

    if (fegetenv(&fenv))
    {
        return -1;
    }
    old_excepts = ~fenv.__control & FE_ALL_EXCEPT;

    // mask
    fenv.__control |= new_excepts;
    fenv.__mxcsr   |= new_excepts << 7;

    return fesetenv(&fenv) ? -1 : old_excepts;
}
#endif


} // namespace detail

} // namespace slowmath


#endif // INCLUDED_SLOWMATH_DETAIL_FENV_HPP_
