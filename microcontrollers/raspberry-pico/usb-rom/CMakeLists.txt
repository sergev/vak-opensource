
cmake_minimum_required(VERSION 3.15)

# Include build functions from Pico SDK
set(PICO_PLATFORM rp2040)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(usb_rom C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)

# Fix waveshare_rp2040_zero USB unrecognize after unplug and replug
# https://github.com/raspberrypi/pico-sdk/pull/1421
add_compile_definitions(PICO_FLASH_SPI_CLKDIV=4)

# Initialize the SDK
set(PICO_BOARD waveshare_rp2040_zero)
pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.c
    msc_disk.c
    usb_descriptors.c
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(${PROJECT_NAME} PUBLIC
    .
)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib
    pico_unique_id
    tinyusb_device
    tinyusb_board
    hardware_pio
)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(${PROJECT_NAME} PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(${PROJECT_NAME})

# 'make upload' to program the binary
add_custom_target(upload
    picotool load -f -x ${PROJECT_NAME}.uf2
    COMMENT "picotool load -f -x ${PROJECT_NAME}.uf2"
    DEPENDS ${PROJECT_NAME}
)
