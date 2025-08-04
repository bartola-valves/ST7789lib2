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

project(ST7789lib2 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()



# Add executable. Default name is the project name, version 0.1

add_executable(ST7789lib2 ST7789lib2.cpp lib/st7789.cpp lib/gfx.cpp )

pico_set_program_name(ST7789lib2 "ST7789lib2")
pico_set_program_version(ST7789lib2 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ST7789lib2 1)
pico_enable_stdio_usb(ST7789lib2 0)

# pico UART pins changed to 16 and 17
target_compile_definitions(ST7789lib2 PRIVATE
	PICO_DEFAULT_UART_TX_PIN=16
	PICO_DEFAULT_UART_RX_PIN=17
	)

# Add the standard library to the build
target_link_libraries(ST7789lib2
        pico_stdlib
        hardware_spi
        hardware_dma)


# Add the standard include files to the build
target_include_directories(ST7789lib2 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib
)

pico_add_extra_outputs(ST7789lib2)

