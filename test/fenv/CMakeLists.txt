
# slowmath C++ library
# Author: Moritz Beutel
# slowmath fenv unit tests


cmake_minimum_required(VERSION 3.20)


include(TargetCompileSettings)

# test target
add_executable(test-slowmath-fenv
    "test-fenv.cpp"
)

# compiler settings
target_compile_features(test-slowmath-fenv PRIVATE cxx_std_14)
cmakeshift_target_compile_settings(test-slowmath-fenv
    SOURCE_FILE_ENCODING "UTF-8"
    FP_MODEL "strict"  # to prevent our fenv tests from being broken by the optimizer
)
if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/EH[ascr]+")
        string(REGEX REPLACE "/EH[ascr]+" "" CMAKE_CXX_FLAGS_NEW "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_NEW} /EHa") # /EHa needed for calling `_set_se_translator()`
    endif()
elseif(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(test-slowmath-fenv PRIVATE -fnon-call-exceptions)  # needed to permit throwing exceptions in signal handlers
endif()

# dependencies
target_link_libraries(test-slowmath-fenv
    PRIVATE
        test-slowmath-settings
)

# register tests
add_test(NAME test-slowmath-fenv COMMAND test-slowmath-fenv)
set_property(TEST test-slowmath-fenv PROPERTY FAIL_REGULAR_EXPRESSION "Sanitizer")
