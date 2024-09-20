#
# Code borrowed from: https://duan.ca/2024/08/swift-mlir-cmake/
#
cmake_minimum_required(VERSION 3.22)

# Note we include "C" here, without it there'd be a build error 🤷
project(swift-mlir LANGUAGES C CXX Swift)

# This is where llvm-config comes to play
find_package(MLIR REQUIRED CONFIG)

include_directories(${MLIR_INCLUDE_DIRS})

# Include our modulemap
include_directories(cmlir)

# I can't believe this is all it takes to make a Swift dylib!
add_library(MLIRSwift SHARED lib.swift)

# Wasted a lot of time on figuring out the right library to link T-T
target_link_libraries(MLIRSwift PRIVATE MLIRCAPIIR)

# Nothing special here
add_executable(myapp main.swift)
target_link_libraries(myapp PRIVATE MLIRSwift)
