# slowmath

[![Language](https://img.shields.io/badge/language-C%2B%2B14-blue)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
 [![License](https://img.shields.io/badge/license-Boost-green)](https://opensource.org/licenses/BSL-1.0)
 [![Build Status](https://dev.azure.com/moritzbeutel/slowmath/_apis/build/status/mbeutel.slowmath?branchName=master)](https://dev.azure.com/moritzbeutel/slowmath/_build/latest?definitionId=4&branchName=master)
 [![Azure DevOps tests](https://img.shields.io/azure-devops/tests/moritzbeutel/slowmath/4)](https://dev.azure.com/moritzbeutel/slowmath/_testManagement/runs)

*slowmath* is a header-only C++ library of checked arithmetic routines.


## Contents

- [Introduction](#introduction)
  - [Example usage](#example-usage)
  - [Motivation](#motivation)
  - [Why *slowmath*?](#why-slowmath)
- [Documentation](#documentation)
  - [Basic arithmetic operations](#basic-arithmetic-operations)
  - [Extended arithmetic operations](#extended-arithmetic-operations)
  - [Integer factorization](#integer-factorization)
  - [Number theory](#number-theory)
  - [Bit operations](#bit-operations)
  - [Floating-point environment](#floating-point-environment)
- [Supported platforms](#supported-platforms)
- [Dependencies](#dependencies)
- [Use and installation](#use-and-installation)
- [Alternatives](#alternatives)
- [References](#references)
- [License](#license)


## Introduction

### Example usage

```c++
#include <slowmath/arithmetic.hpp>

std::size_t computeBufferSize(std::size_t numElements, std::size_t elementSize)
{
        // throws `std::system_error(std::errc::value_too_large)` on overflow
    return slowmath::multiply_checked(numElements, elementSize);
}
```

### Motivation

C++ is infamous for the abundance of [undefined behavior](https://en.cppreference.com/w/cpp/language/ub) in language and standard
library. Perhaps most notably, signed integer overflow is undefined in C++. This defeats most naïve approaches at guarding against
overflow:

```c++
// bad code
int faultyCheckedAdd(int a, int b)
{
    assert(!(a > 0 && b > 0 && a + b < 0)   // Idea: if both values have equal sign, and if adding them
        && !(a < 0 && b < 0 && a + b > 0)); // changes the sign, the value must have overflown.
    return a + b;
}
```

This code tries to exploit the fact that most *hardware implementation* of signed integers wraps around on overflow. However,
because signed integer overflow is undefined, the compiler may legally assume it never happens. As a consequence, compilers may
simply ([elide the naïve overflow check](https://gcc.godbolt.org/z/m8ND9H)):

```asm
faultyCheckedAdd(int, int):
        lea     eax, [rdi+rsi]
        ret
```

The goal of this library is to provide a set of common arithmetic routines with overflow checks.

### Why *slowmath*?

Other libraries for checked arithmetic in C and C++ exist, but
[none of them quite matched what I wanted](https://en.wikipedia.org/wiki/Not_invented_here). *slowmath* was built with the
following goals:


- Checked aritmetic **operations** for the **existing** integer types, not a new set of integer types with checked operations.

  It is reasonable to have runtime checks for *all* arithmetic operations when running unit tests, but I recommend using
  [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) rather than rewriting your code to use
  safe arithmetic everywhere.

  If you really want every operation to be checked even in production code, you probably want to use
  [Boost.SafeNumerics](https://github.com/boostorg/safe_numerics).

- **Generic routines**.

  I want a single function template called `multiply_checked()` rather than a host of type-encoded functions like
  [`UIntPtrMult()`](https://docs.microsoft.com/en-us/windows/win32/api/intsafe/nf-intsafe-uintptrmult) or
  [`psnip_safe_int16_mul()`](https://github.com/nemequ/portable-snippets/tree/master/safe-math).

- **`constexpr` support.** All checked arithmetic routines should be `constexpr`; an overflow in a compile-time computation
  should be a compile-time error.

- **Portable code.**

  Most hardware implementations have support for detecting integer overflow, e.g. x86 sets the
  [overflow flag](https://en.wikipedia.org/wiki/Overflow_flag) if an overflow occurs.
  But [as John Regehr points out](https://blog.regehr.org/archives/1139), it is surprisingly difficult to get compilers to
  generate efficient code for overflow checks. The most reliable way to get efficient codegen is to use
  [compiler intrinsics](https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html)
  or [platform-specific libraries](https://docs.microsoft.com/en-us/windows/win32/api/intsafe/). The downside is that these are
  not portable and typically not `constexpr`.

  *slowmath* favors portability over efficiency; we want efficient codegen where possible, but there should always be a
  portable ISO C++-compliant fallback.

- **Precondition checks.**

  Many arithmetic operations have preconditions, e.g. division requires that the divisor be ≠ 0. Unlike overflows, precondition
  violations are always programmer errors. All arithmetic functions in *slowmath* check their preconditions with `gsl_Expects()`.

- **Flexible error handling.**

  For arithmetic operations that can fail, *slowmath* provides functions with different error handling semantics:

  - `square()`: does not check for overflow
  - `square_checked()`: throws `std::system_error` on overflow
  - `try_square()`: returns a value/error-code aggregate
  - `square_failfast()`: checks for overflow using `gsl_Expects()` precondition assertions


## Documentation

*slowmath* provides the following sets of functions:

- [Basic arithmetic operations](#basic-arithmetic-operations) for integers: addition, subtraction, multiplication, division, modulo
- [Extended arithmetic operations](#extended-arithmetic-operations) for integers: square, exponentiation, rounding, logarithm
- [Integer factorization](#integer-factorization)
- [Number theory](#number-theory): greatest common divisor, least common multiple
- [Bit operations](#bit-operations): shift left, shift right
- [Floating-point environment](#floating-point-environment): trapping floating-point exceptions

### Basic arithmetic operations

Header file: `<slowmath/arithmetic.hpp>`

| function                                                                                              | preconditions  | result     |
| ----------------------------------------------------------------------------------------------------- | -------------- | ---------- |
| `absi(a)` <br> `absi_checked(a)` <br> `absi_failfast(a)` <br> `try_absi(a)`                           | a ∊ ℤ          | |a|        |
| `negate_checked(a)` <br> `negate_failfast(a)` <br> `try_negate(a)`                                    | a ∊ ℤ          | -a         |
| `add_checked(a, b)` <br> `add_failfast(a, b)` <br> `try_add(a, b)`                                    | a,b ∊ ℤ        | a + b      |
| `subtract_checked(a, b)` <br> `subtract_failfast(a, b)` <br> `try_subtract(a, b)`                     | a,b ∊ ℤ        | a - b      |
| `multiply_checked(a, b)` <br> `multiply_failfast(a, b)` <br> `try_multiply(a, b)`                     | a,b ∊ ℤ        | a ∙ b      |
| `divide(n, d)` <br> `divide_checked(n, d)` <br> `divide_failfast(n, d)` <br> `try_divide(n, d)`       | n,d ∊ ℤ, d ≠ 0 | n ÷ d      |
| `modulo(n, d)` <br> `modulo_checked(n, d)` <br> `modulo_failfast(n, d)` <br> `try_modulo(n, d)`       | n,d ∊ ℤ, d ≠ 0 | n mod d    |

### Extended arithmetic operations

Header file: `<slowmath/arithmetic.hpp>`

| function                                                                                            | preconditions          | result          |
| --------------------------------------------------------------------------------------------------- | ---------------------- | --------------- |
| `square(a)` <br> `square_checked(a)` <br> `square_failfast(a)` <br> `try_square(a)`                 | a ∊ ℤ                  | a²              |
| `powi(b, e)` <br> `powi_checked(b, e)` <br> `powi_failfast(b, e)` <br> `try_powi(b, e)`             | b ∊ ℤ, e ∊ ℕ₀          | bᵉ              |
| `floori(x, d)`                                                                                      | x ∊ ℕ₀, d ∊ ℕ, d ≠ 0   | ⌊x ÷ d⌋ ∙ d     |
| `ceili(x, d)` <br> `ceili_checked(x, d)` <br> `ceili_failfast(x, d)` <br> `try_ceili(x, d)`         | x ∊ ℕ₀, d ∊ ℕ, d ≠ 0   | ⌈x ÷ d⌉ ∙ d     |
| `ratio_floori(n, d)`                                                                                | n ∊ ℕ₀, d ∊ ℕ, d ≠ 0   | ⌊n ÷ d⌋         |
| `ratio_ceil(n, d)`                                                                                  | n ∊ ℕ₀, d ∊ ℕ, d ≠ 0   | ⌈n ÷ d⌉         |
| `log_floori(x, b)`                                                                                  | x,b ∊ ℕ, x > 0, b > 1  | ⌊log x ÷ log b⌋ |
| `log_ceil(x, b)`                                                                                    | x,b ∊ ℕ, x > 0, b > 1  | ⌈log x ÷ log b⌉ |

### Integer factorization

Header file: `<slowmath/arithmetic.hpp>`

| function                                                                                                                                        | preconditions                    | result                                               |
| ----------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------------- | ---------------------------------------------------- |
| `factorize_floori(x, b)`                                                                                                                        | x,b ∊ ℕ, x > 0, b > 1            | (r,e) such that x = bᵉ + r with r ≥ 0 minimal        |
| `factorize_ceili(x, b)` <br> `factorize_ceili_checked(x, b)` <br> `factorize_ceili_failfast(x, b)` <br> `try_factorize_ceili(x, b)`             | x,b ∊ ℕ, x > 0, b > 1            | (r,e) such that x = bᵉ - r with r ≥ 0 minimal        |
| `factorize_floori(x, a, b)`                                                                                                                     | x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b | (r,i,j) such that x = aⁱ ∙ bʲ + r with r ≥ 0 minimal |
| `factorize_ceili(x, a, b)` <br> `factorize_ceili_checked(x, a, b)` <br> `factorize_ceili_failfast(x, a, b)` <br> `try_factorize_ceili(x, a, b)` | x,a,b ∊ ℕ, x > 0, a,b > 1, a ≠ b | (r,i,j) such that x = aⁱ ∙ bʲ - r with r ≥ 0 minimal |

### Number theory

Header file: `<slowmath/arithmetic.hpp>`

| function                                                                       | preconditions | result                             |
| ------------------------------------------------------------------------------ | ------------- | ---------------------------------- |
| `gcd_checked(a, b)` <br> `gcd_failfast(a, b)` <br> `try_gcd(a, b)`             | a,b ∊ ℤ       | greatest common divisor of a and b |
| `lcm_checked(a, b)` <br> `lcm_failfast(a, b)` <br> `try_lcm(a, b)`             | a,b ∊ ℤ       | least common multiple of a and b   |

**Note:** Like [`std::gcd()`](https://en.cppreference.com/w/cpp/numeric/gcd) and [`std::lcm()`](https://en.cppreference.com/w/cpp/numeric/lcm), these functions are supported only for C++17 and higher.

### Bit operations

Header file: `<slowmath/arithmetic.hpp>`

| function                                                                                                              | preconditions | result                                    |
| --------------------------------------------------------------------------------------------------------------------- | ------------- | ----------------------------------------- |
| `shift_left(x, s)` <br> `shift_left_checked(x, s)` <br> `shift_left_failfast(x, s)` <br> `try_shift_left(x, s)`       | x,s ∊ ℕ₀      | x ∙ 2ˢ (i.e. x left-shifted by s bits)    |
| `shift_right(x, s)` <br> `shift_right_checked(x, s)` <br> `shift_right_failfast(x, s)` <br> `try_shift_right(x, s)`   | x,s ∊ ℕ₀      | ⌊x ÷ 2ˢ⌋ (i.e. x right-shifted by s bits) |

**Note:** The result of right-shifting negative numbers with the built-in arithmetic shift operator is valid but
implementation-dependent. Unlike the built-in shift operator, `shift_right()` does not support negative operands.

### Floating-point environment

Header file: `<slowmath/fenv.hpp>`

- ```c++
  void
  fe_set_trapping_exceptions(int excepts);
  ```

  Sets hardware exception traps for the floating-point exceptions specified by the given mask value.

  The admissible mask values are defined as [`FE_*`](https://en.cppreference.com/w/cpp/numeric/fenv/FE_exceptions) in standard
  header [`<cfenv>`](https://en.cppreference.com/w/cpp/header/cfenv).  
  If an exception flag bit is set, the corresponding exception will be trapped; if the bit is clear, the exception will be masked.

- ```c++
  int
  fe_get_trapping_exceptions(void);
  ```
  Returns the bitmask of all floating-point exceptions for which trapping is currently enabled.
 
  The admissible mask values are defined as [`FE_*`](https://en.cppreference.com/w/cpp/numeric/fenv/FE_exceptions) in standard
  header [`<cfenv>`](https://en.cppreference.com/w/cpp/header/cfenv).  

Floating-point exceptions are usually *silent*, i.e. they only set an exception state in the floating-point unit but do not affect
the flow of execution. Using `fe_set_trapping_exceptions()`, the FPU can be configured to trigger a hardware exception for certain
floating-point exceptions, which raises a `SIGFPE` signal on POSIX platforms or a SEH exception on Windows.

Note that, if the compiler is configured to perform aggressive floating-point optimizations, floating-point exceptions may be
raised at seemingly unrelated points in the code, or may not be raised at all. For the highest fidelity in error diagnosis,
configure your compiler to perform only IEEE-754-conforming floating-point optimizations (cf. compiler options for
[MSVC](https://docs.microsoft.com/en-us/cpp/build/reference/fp-specify-floating-point-behavior),
[GCC](https://gcc.gnu.org/wiki/FloatingPointMath),
[Clang](https://clang.llvm.org/docs/UsersManual.html#controlling-floating-point-behavior),
[ICC](https://software.intel.com/en-us/cpp-compiler-developer-guide-and-reference-floating-point-optimizations),
[NVCC](https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html#options-for-steering-gpu-code-generation-use_fast_math)).


## Supported platforms

The checked arithmetic operations in `<slowmath/arithmetic.hpp>` work with every compliant C++14 compiler and have no platform
dependencies.

The floating-point environment operations in `<slowmath/fenv.hpp>` are currently implemented for Windows, Linux, and MacOS.

*slowmath* is [continuously tested](https://dev.azure.com/moritzbeutel/slowmath/_build/latest?definitionId=4&branchName=master)
with the following compilers and platforms:

Compiler             | OS              | Platforms | Versions                  |
--------------------:|:----------------|-----------|--------------------------:|
GCC                  | Linux           | x64       | 6 and newer               |
GCC                  | MacOS           | x64       | 6 and newer               |
Clang                | Linux           | x64       | 5 and newer               |
Clang                | Windows         | x64       | 9 and newer               |
MSVC (Visual Studio) | Windows         | x86, x64  | 19.1 (VS 2017) and newer  |
AppleClang (Xcode)   | MacOS           | x64       | 9.1 (Xcode 9.3) and newer |
NVCC (CUDA Toolkit)  | Linux, Windows  | x64       | 10.2 and newer            |


## Dependencies

* [CMakeshift](https://github.com/mbeutel/CMakeshift)
* [gsl-lite](https://github.com/gsl-lite/gsl-lite)
* for unit tests: [Catch2](https://github.com/catchorg/Catch2)


## Use and installation

*slowmath* comes with a CMake package config file that exports a target `slowmath::slowmath`:

```cmake
find_package(slowmath 0.3 REQUIRED)

add_executable(my_proj "main.cpp")
target_link_libraries(my_proj PRIVATE slowmath::slowmath)
```

*slowmath* is most easily installed with [Vcpkg](https://github.com/microsoft/vcpkg/):

```
vcpkg install slowmath
```

Alternatively, you can configure the repository manually:

```
git clone git@github.com:mbeutel/slowmath.git
cd slowmath
mkdir build
cd build
cmake ..
```

To use the *slowmath* build directory as a CMake package, specify `-Dslowmath_DIR:FILEPATH=<slowmath-dir>/build` on the command
line when configuring your project with CMake.


## Alternatives

[1] *safe-math* in [https://github.com/nemequ/portable-snippets](https://github.com/nemequ/portable-snippets/tree/master/safe-math) by [Evan Nemerson](https://github.com/nemequ)  
[2] [Boost.SafeNumerics](https://github.com/boostorg/safe_numerics) by [Robert Ramey](https://github.com/robertramey)  


## References

[1] [SEI CERT C Coding Standard, Rule 04. Integers (INT)](https://wiki.sei.cmu.edu/confluence/x/tNUxBQ). The implementation of
*slowmath* mostly follows the techniques presented here.  
[2] W. Dietz et al., [Understanding Integer Overflow in C/C++](http://www.cs.utah.edu/~regehr/papers/tosem15.pdf), 2015
[3] Project Nayuki, [Undefined behavior in C and C++ programs](https://www.nayuki.io/page/undefined-behavior-in-c-and-cplusplus-programs)


## License

Code licensed under the [Boost Software License](LICENSE.txt).
