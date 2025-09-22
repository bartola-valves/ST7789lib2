# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Change project name to match our new target
project(ST7789v_hello_test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable for simple medium font test - ONLY TARGET
add_executable(ST7789v_hello_test
    ST7789v_hello_test.cpp
    lib/st7789v.c
)

# Link libraries
target_link_libraries(ST7789v_hello_test
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# Enable UART output (USB disabled for this test)
pico_enable_stdio_usb(ST7789v_hello_test 0)
pico_enable_stdio_uart(ST7789v_hello_test 1)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(ST7789v_hello_test)

