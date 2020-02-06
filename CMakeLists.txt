
# slowmath C++ library
# Author: Moritz Beutel


cmake_minimum_required(VERSION 3.14)

# Define the project.
project(slowmath
    VERSION 0.2.0
    LANGUAGES CXX)

# Define build options.
option(SLOWMATH_BUILD_TESTS "Build tests" OFF)
option(SLOWMATH_BUILD_CUDA_TESTS "Build CUDA tests" OFF)
option(CMAKE_EXPORT_PACKAGE_REGISTRY "Export to user package registry" OFF)

# Include target definitions.
add_subdirectory(src)
if(SLOWMATH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
    if(SLOWMATH_BUILD_CUDA_TESTS)
        enable_language(CUDA)
        add_subdirectory(test/cuda)
    endif()
endif()

# Install.
find_package(CMakeshift 3.8 REQUIRED)
include(CMakeshift/InstallBasicPackageFiles)
cmakeshift_install_basic_package_files(slowmath
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
    DEPENDENCIES
        "gsl-lite 0.36")
