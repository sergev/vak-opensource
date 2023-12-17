cmake_minimum_required(VERSION 3.13.2)
project(wxdemo VERSION 0.0.1 DESCRIPTION "Demo of minimal WxWidgets application")

find_package(wxWidgets REQUIRED COMPONENTS net core base)
if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()
add_executable(minimal minimal.cpp)
target_link_libraries(minimal ${wxWidgets_LIBRARIES})
