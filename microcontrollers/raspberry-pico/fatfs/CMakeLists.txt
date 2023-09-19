# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(fatfs_demo_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
    main.c
    hw_config.c
    fatfs.c
    unicode.c
    diskio.c
    my_debug.c
    sd_card.c
    sd_spi.c
    spi.c
    crc.c
    demo_logging.c
)

target_include_directories(${PROJECT_NAME} BEFORE PUBLIC
    $ENV{PICO_SDK_PATH}/src/common/pico_stdlib/include
    $ENV{PICO_SDK_PATH}/src/common/pico_base/include
    $ENV{PICO_SDK_PATH}/src/common/pico_sync/include
    $ENV{PICO_SDK_PATH}/src/common/pico_time/include
    $ENV{PICO_SDK_PATH}/src/common/pico_util/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/pico_platform/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/pico_stdio/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_gpio/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_timer/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_base/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_sync/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_irq/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_dma/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_spi/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_uart/include
    $ENV{PICO_SDK_PATH}/src/rp2_common/hardware_rtc/include
    $ENV{PICO_SDK_PATH}/src/rp2040/hardware_structs/include
    $ENV{PICO_SDK_PATH}/src/rp2040/hardware_regs/include
    ${CMAKE_BINARY_DIR}/generated/pico_base
)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME}
    hardware_spi
    hardware_dma
    hardware_rtc
    pico_stdlib
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# 'make upload' to program the binary
add_custom_target(upload
    picotool load -f -x ${PROJECT_NAME}.uf2
    COMMENT "picotool load -f -x ${PROJECT_NAME}.uf2"
    DEPENDS ${PROJECT_NAME}
)
