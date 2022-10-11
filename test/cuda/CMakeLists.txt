
# slowmath C++ library
# Author: Moritz Beutel
# slowmath CUDA unit tests


cmake_minimum_required(VERSION 3.20)


include(TargetCompileSettings)

# CUDA tests
add_executable(test-slowmath-cuda14)
target_compile_features(test-slowmath-cuda14 PRIVATE cxx_std_14 cuda_std_14)
cmakeshift_target_compile_settings(test-slowmath-cuda14 SOURCE_FILE_ENCODING "UTF-8")
target_link_libraries(test-slowmath-cuda14
    PRIVATE
        test-slowmath-settings
)
add_test(NAME test-slowmath-cuda14 COMMAND test-slowmath-cuda14)
set_property(TEST test-slowmath-cuda14 PROPERTY FAIL_REGULAR_EXPRESSION "Sanitizer")

# All the sources in the C++14 test target should also compile with NVCC.
get_target_property(SOURCES_CUDA14 test-slowmath-cxx14 SOURCES)
list(TRANSFORM SOURCES_CUDA14 PREPEND "../")
target_sources(test-slowmath-cuda14 PRIVATE ${SOURCES_CUDA14})
set_property(SOURCE ${SOURCES_CUDA14} PROPERTY LANGUAGE CUDA)
