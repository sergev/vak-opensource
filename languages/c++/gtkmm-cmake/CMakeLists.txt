cmake_minimum_required(VERSION 3.15)
project(example_project VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig)
pkg_check_modules(GTKMM_VARS REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_executable(example_project
    helloworld.cpp
    main.cpp
)
target_link_libraries(example_project PRIVATE PkgConfig::GTKMM_VARS)
