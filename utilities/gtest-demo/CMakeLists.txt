cmake_minimum_required(VERSION 3.13)
project(demo VERSION 0.0 DESCRIPTION "Demo of googletest")

#
# Use C++14, treat warnings as errors
#
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Werror")

#
# Download and unpack googletest at configure time
#
configure_file(GTestLists.txt googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

#
# Add googletest directly to our build.
# This defines the gtest and gtest_main targets.
#
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

#
# Now simply link against gtest or gtest_main as needed.
#
add_executable(test
    CharUtil.cpp
    StringUtil.cpp
    CharUtilTest.cpp
    SoundexTest.cpp
    StringUtilTest.cpp
)
target_link_libraries(test gtest_main)
