cmake_minimum_required(VERSION 3.15)
project(metal_cpp_demo LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set macOS deployment target
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment version")

# Find metal-cpp include directory from mlx package
# Try to find via Homebrew first
find_program(BREW_EXECUTABLE brew)
if(BREW_EXECUTABLE)
    execute_process(
        COMMAND ${BREW_EXECUTABLE} --prefix mlx
        OUTPUT_VARIABLE MLX_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(MLX_PREFIX)
        set(MLX_METAL_CPP_DIR "${MLX_PREFIX}/include/metal_cpp")
        if(EXISTS "${MLX_METAL_CPP_DIR}/Metal/Metal.hpp")
            set(METAL_CPP_INCLUDE_DIR "${MLX_METAL_CPP_DIR}")
        endif()
    endif()
endif()

# If not found via Homebrew, search common locations
if(NOT METAL_CPP_INCLUDE_DIR)
    find_path(METAL_CPP_INCLUDE_DIR
        NAMES Metal/Metal.hpp
        PATHS
            /opt/homebrew/opt/mlx/include/metal_cpp
            /opt/homebrew/Cellar/mlx/*/include/metal_cpp
            /usr/local/opt/mlx/include/metal_cpp
            /usr/local/Cellar/mlx/*/include/metal_cpp
        PATH_SUFFIXES metal_cpp
    )
endif()

# Error if not found
if(NOT METAL_CPP_INCLUDE_DIR)
    message(FATAL_ERROR "metal-cpp headers not found. Please install mlx package via Homebrew: brew install mlx")
endif()

message(STATUS "Found metal-cpp headers at: ${METAL_CPP_INCLUDE_DIR}")

# Add executable with C++ files
add_executable(metal_cpp_demo
    metal_renderer.cpp
    app_main.mm
)

# Include directories
target_include_directories(metal_cpp_demo PRIVATE ${METAL_CPP_INCLUDE_DIR})

# app_main.mm is automatically recognized as Objective-C++ by CMake

# Link frameworks (CMake should find them automatically on macOS)
find_library(METAL_FRAMEWORK Metal)
find_library(FOUNDATION_FRAMEWORK Foundation)
find_library(QUARTZCORE_FRAMEWORK QuartzCore)
find_library(APPKIT_FRAMEWORK AppKit)
find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)

target_link_libraries(metal_cpp_demo
    ${METAL_FRAMEWORK}
    ${FOUNDATION_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${APPKIT_FRAMEWORK}
    ${COREFOUNDATION_FRAMEWORK}
)

# Compile Metal shader
set(SHADER_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/shader.metal")
set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shader.air")
set(SHADER_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/shader.metallib")

# Compile Metal shader to AIR
add_custom_command(
    OUTPUT ${SHADER_OUTPUT}
    COMMAND xcrun -sdk macosx metal -c ${SHADER_SOURCE} -o ${SHADER_OUTPUT}
    DEPENDS ${SHADER_SOURCE}
    COMMENT "Compiling Metal shader to AIR"
)

# Link AIR to Metal library
add_custom_command(
    OUTPUT ${SHADER_LIBRARY}
    COMMAND xcrun -sdk macosx metallib ${SHADER_OUTPUT} -o ${SHADER_LIBRARY}
    DEPENDS ${SHADER_OUTPUT}
    COMMENT "Linking Metal shader library"
)

# Add shader library as dependency
add_custom_target(shader_lib DEPENDS ${SHADER_LIBRARY})
add_dependencies(metal_cpp_demo shader_lib)

# Copy shader source to build directory so the executable can find it
add_custom_command(TARGET metal_cpp_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SHADER_SOURCE}
    ${CMAKE_CURRENT_BINARY_DIR}/shader.metal
)
