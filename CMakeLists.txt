cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(modxo)

# Creates a pico-sdk subdirectory
pico_sdk_init()

add_executable(${PROJECT_NAME})

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/lpc/lpc_comm.pio)

#Source files
target_sources(${PROJECT_NAME} PRIVATE 
    src/main.c 
    src/lpc/lpc_comm.pio 
    src/lpc/lpc_interface.c
    src/lpc/lpc_log.c
    src/flash_rom/flash_rom.c
    src/superio/LPC47M152.c
    src/superio/UART_16550.c
)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
    hardware_pio
    hardware_dma
    pico_multicore
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

