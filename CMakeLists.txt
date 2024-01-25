
# slowmath C++ library
# Author: Moritz Beutel


cmake_minimum_required(VERSION 3.20)

# Define the project.
project(slowmath
    VERSION 0.4
    LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Define build options.
option(SLOWMATH_BUILD_TESTING "Build tests" OFF)
option(SLOWMATH_BUILD_TESTING_CUDA "Build CUDA tests" OFF)

# Include target definitions.
add_subdirectory(src)
if(SLOWMATH_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(test/fenv)
    if(SLOWMATH_BUILD_TESTING_CUDA)
        enable_language(CUDA)
        add_subdirectory(test/cuda)
    endif()
endif()

# Install.
include(InstallBasicPackageFiles)
cmakeshift_install_basic_package_files(slowmath
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
    DEPENDENCIES
        "gsl-lite 0.40"
)
