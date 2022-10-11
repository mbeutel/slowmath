
# slowmath C++ library
# Author: Moritz Beutel
# slowmath unit tests


cmake_minimum_required(VERSION 3.20)

# dependencies
find_package(gsl-lite 0.40 REQUIRED)
find_package(Catch2 3 REQUIRED)

include(TargetCompileSettings)

# common settings target
add_library(test-slowmath-settings INTERFACE)
add_library(test-slowmath-pch INTERFACE)
target_compile_definitions(test-slowmath-settings
    INTERFACE
        #CATCH_CONFIG_CONSOLE_WIDTH=120
        #CATCH_CLARA_CONFIG_CONSOLE_WIDTH=120
        gsl_CONFIG_CONTRACT_CHECKING_AUDIT  # run audit-level checks
        gsl_CONFIG_CONTRACT_VIOLATION_THROWS  # make gsl-lite's precondition assertions throw `gsl::fail_fast` on failure
)
target_link_libraries(test-slowmath-settings
    INTERFACE
        gsl::gsl-lite-v1
        Catch2::Catch2WithMain
        slowmath
)
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
        <catch2/catch_test_macros.hpp>
)

# C++14 tests
add_executable(test-slowmath-cxx14
    "legacy-tests.cpp"
    "test-arithmetic.cpp"
    "test-bits.cpp"
    "test-factorize.cpp"
    "test-pow-log.cpp"
    "test-round.cpp"
)
target_compile_features(test-slowmath-cxx14 PRIVATE cxx_std_14)
cmakeshift_target_compile_settings(test-slowmath-cxx14 SOURCE_FILE_ENCODING "UTF-8")
target_link_libraries(test-slowmath-cxx14
    PRIVATE
        test-slowmath-settings
        test-slowmath-pch
)
add_test(NAME test-slowmath-cxx14 COMMAND test-slowmath-cxx14)
set_property(TEST test-slowmath-cxx14 PROPERTY FAIL_REGULAR_EXPRESSION "Sanitizer")

# C++17 tests
if(cxx_std_17 IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    add_executable(test-slowmath-cxx17
        "test-gcd-lcm.cpp"
    )
    target_compile_features(test-slowmath-cxx17 PRIVATE cxx_std_17)
    cmakeshift_target_compile_settings(test-slowmath-cxx17 SOURCE_FILE_ENCODING "UTF-8")
    target_link_libraries(test-slowmath-cxx17
        PRIVATE
            test-slowmath-settings
    )
    add_test(NAME test-slowmath-cxx17 COMMAND test-slowmath-cxx17)
    set_property(TEST test-slowmath-cxx17 PROPERTY FAIL_REGULAR_EXPRESSION "Sanitizer")
endif()
