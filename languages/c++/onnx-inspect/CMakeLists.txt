cmake_minimum_required(VERSION 3.13.2)
project(onnx-inspect VERSION 0.0.1 DESCRIPTION "Inspect ONNX models")

#--------------------------------------------------------------
# Options
#
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_MACOSX_RPATH ON)

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
set(CMAKE_INSTALL_RPATH '')

# Do not add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--------------------------------------------------------------
# C++ flags
#
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#--------------------------------------------------------------
# onnx-inspect
#
add_executable(onnx-inspect
    inspect.cpp
)
include_directories(
    /usr/local/include
    /opt/homebrew/include
)
target_link_directories(onnx-inspect PRIVATE
    /usr/local/lib
    /opt/homebrew/lib
)
target_link_libraries(onnx-inspect onnxruntime)

install(TARGETS onnx-inspect DESTINATION bin)
