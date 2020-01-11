
# slowmath C++ library
# Author: Moritz Beutel
# slowmath CUDA unit tests


cmake_minimum_required(VERSION 3.14)

# dependencies
find_package(Catch2 2.9 REQUIRED)
find_package(CMakeshift 3.7 REQUIRED)
find_package(gsl-lite 0.35.5 REQUIRED)

include(CMakeshift/TargetCompileSettings)

# CUDA test target
add_executable(test-slowmath-cuda)

# compiler settings
target_compile_features(test-slowmath-cuda PRIVATE cxx_std_14)
set_target_properties(test-slowmath-cuda PROPERTIES CUDA_STANDARD 14)
cmakeshift_target_compile_settings(test-slowmath-cuda PRIVATE default)

# test properties
target_compile_definitions(test-slowmath-cuda
    INTERFACE
        CATCH_CONFIG_CONSOLE_WIDTH=120
        CATCH_CLARA_CONFIG_CONSOLE_WIDTH=120)

# test dependencies
target_link_libraries(test-slowmath-cuda
    PRIVATE
        test-slowmath-runner
        gsl::gsl-lite-v1
        Catch2::Catch2
        slowmath)

# All the sources in the C++14 test target should also compile with NVCC.
get_target_property(SOURCES_CUDA14 test-slowmath-cxx14 SOURCES)
list(TRANSFORM SOURCES_CUDA14 PREPEND "../")
target_sources(test-slowmath-cuda PRIVATE ${SOURCES_CUDA14})
set_source_files_properties(${SOURCES_CUDA14} PROPERTIES LANGUAGE CUDA)
