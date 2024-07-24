cmake_minimum_required(VERSION 3.8)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_BUILD_TYPE Debug)

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
pico_set_binary_type(${PROJECT_NAME} copy_to_ram)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE HEADERS "${SOURCE_DIR}/*.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})

pico_generate_pio_header(${PROJECT_NAME} ${SOURCE_DIR}/lpc/lpc_comm.pio)

set(DIRECTORIES_TO_GLOB
    "${SOURCE_DIR}"
    "${SOURCE_DIR}/flash_rom"
    "${SOURCE_DIR}/lpc"
    "${SOURCE_DIR}/superio"
    "${SOURCE_DIR}/led"
)
set(SOURCES "")

foreach(dir ${DIRECTORIES_TO_GLOB})
    file(GLOB dir_files "${dir}/*.c")
    list(APPEND SOURCES ${dir_files})
endforeach()

if(WS2812_LED)
    message(STATUS "Compiling with WS2812 support")
    list(APPEND SOURCES "${SOURCE_DIR}/led/ws2812/ws2812_led.c")
    pico_generate_pio_header(${PROJECT_NAME} ${SOURCE_DIR}/led/ws2812/ws2812.pio)
else()
    message(STATUS "Compiling without WS2812 support")
    list(APPEND SOURCES "${SOURCE_DIR}/led/pico/pico_led.c")
endif()

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

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

