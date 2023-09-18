# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(sysreg_usb_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

add_executable(${PROJECT_NAME}
    sysreg_usb.c
)

# pull in common dependencies
target_link_libraries(${PROJECT_NAME} pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# 'make upload' to program the binary
add_custom_target(upload
    picotool load -f -x ${PROJECT_NAME}.uf2
    COMMENT "picotool load -f -x ${PROJECT_NAME}.uf2"
    DEPENDS ${PROJECT_NAME}
)
