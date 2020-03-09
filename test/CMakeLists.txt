
# slowmath C++ library
# Author: Moritz Beutel
# slowmath unit tests


cmake_minimum_required(VERSION 3.14)

# dependencies
find_package(CMakeshift 3.8 REQUIRED)
find_package(gsl-lite 0.36 REQUIRED)
find_package(Catch2 2.11 REQUIRED)

include(CMakeshift/TargetCompileSettings)

# test runner target
add_library(test-slowmath-runner OBJECT "main.cpp")
target_compile_features(test-slowmath-runner PRIVATE cxx_std_14)
cmakeshift_target_compile_settings(test-slowmath-runner PRIVATE default)
target_link_libraries(test-slowmath-runner
    PRIVATE
        Catch2::Catch2)

# common settings target
add_library(test-slowmath-settings INTERFACE)
add_library(test-slowmath-pch INTERFACE)
target_compile_definitions(test-slowmath-settings
    INTERFACE
        CATCH_CONFIG_CONSOLE_WIDTH=120
        CATCH_CLARA_CONFIG_CONSOLE_WIDTH=120
        gsl_CONFIG_CONTRACT_CHECKING_AUDIT # run audit-level checks
        gsl_CONFIG_CONTRACT_VIOLATION_THROWS) # make gsl-lite's precondition assertions throw `gsl::fail_fast` on failure
target_link_libraries(test-slowmath-settings
    INTERFACE
        gsl::gsl-lite-v1
        Catch2::Catch2
        slowmath)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    target_precompile_headers(test-slowmath-pch
        INTERFACE
            # C standard library
            <climits>
            <cstddef>
            <cstdint>
            # C++ standard library
            <array>
            <tuple>
            <limits>
            <sstream>
            <utility>
            <iterator>
            <algorithm>
            <exception>
            <type_traits>
            <system_error>
            # other libraries
            <gsl/gsl-lite.hpp>
            <catch2/catch.hpp>)
endif()

# C++14 tests
add_executable(test-slowmath-cxx14
    "legacy-tests.cpp"
    "test-arithmetic.cpp"
    "test-bits.cpp"
    "test-factorize.cpp"
    "test-gcd-lcm.cpp"
    "test-pow-log.cpp"
    "test-round.cpp")
target_compile_features(test-slowmath-cxx14 PRIVATE cxx_std_14)
cmakeshift_target_compile_settings(test-slowmath-cxx14 PRIVATE default)
target_link_libraries(test-slowmath-cxx14
    PRIVATE
        test-slowmath-runner
        test-slowmath-settings
        test-slowmath-pch)
add_test(NAME test-slowmath-cxx14 COMMAND test-slowmath-cxx14)
set_property(TEST test-slowmath-cxx14 PROPERTY FAIL_REGULAR_EXPRESSION "Sanitizer")

# C++17 tests
if(cxx_std_17 IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    add_executable(test-slowmath-cxx17
        "test-gcd-lcm.cpp")
    target_compile_features(test-slowmath-cxx17 PRIVATE cxx_std_17)
    cmakeshift_target_compile_settings(test-slowmath-cxx17 PRIVATE default)
    target_link_libraries(test-slowmath-cxx17
        PRIVATE
            test-slowmath-runner
            test-slowmath-settings)
    add_test(NAME test-slowmath-cxx17 COMMAND test-slowmath-cxx17)
    set_property(TEST test-slowmath-cxx17 PROPERTY FAIL_REGULAR_EXPRESSION "Sanitizer")
endif()
