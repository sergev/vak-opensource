cmake_minimum_required(VERSION 3.15)
project(opencv VERSION 0.0.1 LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_executable(hello-opencv main.cpp)
target_include_directories(hello-opencv PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(hello-opencv ${OpenCV_LIBS})
