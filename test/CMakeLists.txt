
# slowmath C++ library
# Author: Moritz Beutel
# slowmath unit tests


cmake_minimum_required(VERSION 3.15)

# dependencies
find_package(Catch2 2.9 REQUIRED)
find_package(CMakeshift 3.7 REQUIRED)
find_package(gsl-lite 0.35.3 REQUIRED)

# common settings target
add_library(test-slowmath-settings INTERFACE)

# precompiled headers
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    target_precompile_headers(test-slowmath-settings
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
            <numeric>
            <iterator>
            <algorithm>
            <exception>
            <type_traits>
            <system_error>
            # other libraries
            <gsl/gsl-lite.hpp>
            <catch2/catch.hpp>)
endif()

# test runner target
add_library(test-slowmath-runner OBJECT
    "main.cpp")

# test targets
add_executable(test-slowmath-cxx14
    "legacy-tests.cpp"
    "test-arithmetic.cpp"
    "test-bits.cpp"
    "test-cast.cpp"
    "test-factorize.cpp"
    "test-gcd-lcm.cpp"
    "test-pow-log.cpp"
    "test-round.cpp")
add_executable(test-slowmath-cxx17
    "test-gcd-lcm.cpp")

# compiler settings
include(CMakeshift/TargetCompileSettings)
cmakeshift_target_compile_settings(
    TARGETS test-slowmath-cxx14 test-slowmath-cxx17
    PRIVATE
        default)
target_compile_features(test-slowmath-runner
    PRIVATE
        cxx_std_14)
target_compile_features(test-slowmath-cxx14
    PRIVATE
        cxx_std_14)
target_compile_features(test-slowmath-cxx17
    PRIVATE
        cxx_std_17)

# test properties
target_compile_definitions(test-slowmath-settings
    INTERFACE
        CATCH_CONFIG_CONSOLE_WIDTH=120
        CATCH_CLARA_CONFIG_CONSOLE_WIDTH=120
        gsl_CONFIG_CONTRACT_VIOLATION_THROWS) # this makes GSL's Expects()/Ensures() throw `gsl::fail_fast` on failure

# test dependencies
target_link_libraries(test-slowmath-runner
    PRIVATE
        Catch2::Catch2)
target_link_libraries(test-slowmath-settings
    INTERFACE
        slowmath
        gsl::gsl-lite-v1
        Catch2::Catch2)
target_link_libraries(test-slowmath-cxx14
    PRIVATE
        test-slowmath-runner
        test-slowmath-settings)
target_link_libraries(test-slowmath-cxx17
    PRIVATE
        test-slowmath-runner
        test-slowmath-settings)

# register tests
add_test(NAME test-slowmath-cxx14 COMMAND test-slowmath-cxx14)
add_test(NAME test-slowmath-cxx17 COMMAND test-slowmath-cxx17)
