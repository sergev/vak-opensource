
cmake_minimum_required(VERSION 3.15)

# Include build functions from Pico SDK
set(PICO_PLATFORM rp2040)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(blink C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)

# Initialize the SDK
set(PICO_BOARD waveshare_rp2040_zero)
pico_sdk_init()

add_executable(${PROJECT_NAME}
    blink.c
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib
)

pico_add_extra_outputs(${PROJECT_NAME})

# 'make upload' to program the binary
add_custom_target(upload
    picotool load -f -x ${PROJECT_NAME}.uf2
    COMMENT "picotool load -f -x ${PROJECT_NAME}.uf2"
    DEPENDS ${PROJECT_NAME}
)
