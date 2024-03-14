cmake_minimum_required(VERSION 3.19)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(modxo)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory
pico_sdk_init()

add_executable(${PROJECT_NAME})

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/lpc/lpc_mem_read.pio)

#Source files
target_sources(${PROJECT_NAME} PRIVATE 
    src/main.c 
    src/lpc/lpc_mem_read.pio 
    src/lpc/lpc_interface.c
)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
    hardware_pio
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

