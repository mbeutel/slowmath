
#include <slowmath/fenv.hpp>

#include <cfenv>
#include <cmath>     // for sqrt()
#include <tuple>
#include <utility>   // for exchange()
#include <exception>

// Clang doesn't currently implement /EHa properly, cf. https://groups.google.com/forum/#!topic/llvm-dev/ZcNUP_1550M.
#if defined(_MSC_VER) && !defined(__clang__)
# include <Windows.h>
# include <eh.h>      // for _set_se_translator()
#elif defined(__GNUC__) && defined(__linux__) && !defined(__clang__)
# include <csignal>
#endif

#include <catch2/catch.hpp>


#if defined(_MSC_VER) && !defined(__clang__)
class StructuredException : public std::exception
{
private:
    unsigned seNumber_;

public:
    StructuredException(unsigned _seNumber) noexcept
        : seNumber_(_seNumber)
    {
    }
    unsigned
    seNumber(void) const noexcept
    {
        return seNumber_;
    }
};

class ScopedStructuredExceptionTranslator
{
private:
    _se_translator_function oldTranslator_;

public:
    ScopedStructuredExceptionTranslator(_se_translator_function newTranslator) noexcept
        : oldTranslator_(_set_se_translator(newTranslator))
    {
    }
    ~ScopedStructuredExceptionTranslator(void) noexcept
    {
        if (oldTranslator_ != nullptr)
        {
            _set_se_translator(oldTranslator_);
        }
    }

    ScopedStructuredExceptionTranslator(ScopedStructuredExceptionTranslator&& rhs) // pre-C++17 tax
        : oldTranslator_(std::exchange(rhs.oldTranslator_, nullptr))
    {
    }
    ScopedStructuredExceptionTranslator&
    operator =(ScopedStructuredExceptionTranslator&&) = delete;
};

void translateStructuredExceptionToStdException(unsigned u, EXCEPTION_POINTERS*)
{
    throw StructuredException(u);
}
#elif defined(__GNUC__) && defined(__linux__) && !defined(__clang__)
class FPException : public std::exception
{
};

extern "C"
{

//[[gnu::no_caller_saved_registers]]
void fpSignalHandler(int)
{
    asm(".cfi_signal_frame");
    throw FPException();
}

} // extern "C"
#endif


template <typename T>
void
discard(T) // pre-C++17 tax
{
}

void
divBy0(void)
{
    volatile double zero = 0.;
    volatile double x = 1./zero;
    discard(x);
}

void
inexact(void)
{
    volatile double ten = 10.;
    volatile double x = 1./ten;
    discard(x);
}

void
invalid(void)
{
    volatile double minus1 = -1.;
    volatile double x = std::sqrt(minus1);
    discard(x);
}


TEST_CASE("fenv")
{
#if defined(_MSC_VER) && !defined(__clang__)
    auto scopedExcTranslator = ScopedStructuredExceptionTranslator(translateStructuredExceptionToStdException);
#endif // defined(_MSC_VER) && !defined(__clang__)

    auto excFunc_excCode = GENERATE(
        std::make_tuple(&divBy0, FE_DIVBYZERO),
        std::make_tuple(&inexact, FE_INEXACT),
        std::make_tuple(&invalid, FE_INVALID)
    );
    auto excFunc = std::get<0>(excFunc_excCode); // pre-C++17 tax
    auto excCode = std::get<1>(excFunc_excCode);

    SECTION("can detect floating-point exceptions")
    {
        CHECK(std::feclearexcept(FE_ALL_EXCEPT) == 0);
        REQUIRE_NOTHROW(excFunc());
        REQUIRE(std::fetestexcept(excCode));
        CHECK(std::feclearexcept(FE_ALL_EXCEPT) == 0);
    }

    SECTION("FPU does not raise exception if flag is not set")
    {
        CHECK(std::feclearexcept(FE_ALL_EXCEPT) == 0);
        slowmath::fe_set_trapping_exceptions(FE_ALL_EXCEPT & ~excCode);
        CHECK(slowmath::fe_get_trapping_exceptions() == (FE_ALL_EXCEPT & ~excCode));
        CHECK_NOTHROW(excFunc());
        slowmath::fe_set_trapping_exceptions(0);
    }

    SECTION("FPU raises exception if flag is set")
    {
#if defined(_MSC_VER) && !defined(__clang__)
        slowmath::fe_set_trapping_exceptions(excCode);
        CHECK(slowmath::fe_get_trapping_exceptions() == excCode);
        CHECK_THROWS_AS(excFunc(), StructuredException);
        slowmath::fe_set_trapping_exceptions(0);
#elif defined(__GNUC__) && defined(__linux__) && !defined(__clang__)
        if (excCode == FE_DIVBYZERO)
        {
                // GCC has trouble recovering from more than one SIGFPE, so we only test one.
            std::signal(SIGFPE, fpSignalHandler);
            slowmath::fe_set_trapping_exceptions(excCode);
            CHECK(slowmath::fe_get_trapping_exceptions() == excCode);
            CHECK_THROWS_AS(excFunc(), FPException);
            slowmath::fe_set_trapping_exceptions(0);
        }
#endif
    }
    CHECK(std::feclearexcept(FE_ALL_EXCEPT) == 0);
}
