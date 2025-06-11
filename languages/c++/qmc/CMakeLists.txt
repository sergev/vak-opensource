cmake_minimum_required(VERSION 3.10)
project(qmc)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wshadow")

#
# Enable cppcheck by default
#
#find_program(Cppcheck NAMES cppcheck)
#if(NOT (Cppcheck MATCHES "NOTFOUND"))
#    message(STATUS "Found Cppcheck: ${Cppcheck}")
#    set(CMAKE_CXX_CPPCHECK "${Cppcheck}"
#        "--std=c++17"
#        "--enable=style"
#        "--check-level=exhaustive"
#        "--error-exitcode=1" # Fail on any issues
#        "--inline-suppr" # Enable inline control like // cppcheck-suppress "id"
#        "--quiet" # No progress report messages
#        "--suppress=badBitmaskCheck" # Allow redundant zero operands
#        "--suppress=*:*/_deps/*" # Ignore issues in Googletest
#        "--library=${CMAKE_SOURCE_DIR}/scripts/googletest.xml" # Parse TEST() macro properly
#    )
#endif()

# Download GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    EXCLUDE_FROM_ALL
)
set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
enable_testing()

#
# Minimizer library
#
add_library(minimizer STATIC
    minimizer.cpp
    utils.cpp
)

#
# Tests for minimizer
#
add_executable(minimizer-tests tests.cpp)
target_link_libraries(minimizer-tests minimizer GTest::gtest_main)
gtest_discover_tests(minimizer-tests EXTRA_ARGS --gtest_repeat=1 PROPERTIES TIMEOUT 10)

#
# 'qmc' executable
#
#add_executable(qmc main.cpp)
#target_link_libraries(qmc minimizer)
