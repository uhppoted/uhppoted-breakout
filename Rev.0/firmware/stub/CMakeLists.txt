cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(stub C CXX ASM)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_compile_definitions(VERSION=${VERSION})

set(CORE "../core")
set(LIB  "../lib")

pico_sdk_init()

add_executable(stub
  src/main.c
  src/stub.c
  src/sys.c
  src/hwconfig.c
  src/log.c
)

pico_generate_pio_header(
    stub
    ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio
)

pico_set_program_name(stub "uhppoted-stub")
pico_set_program_version(stub "0.0")

pico_enable_stdio_usb (stub 1)
pico_enable_stdio_uart(stub 0)
pico_add_extra_outputs(stub)

# Ref. https://tinygo.org/docs/reference/microcontrollers/machine/waveshare-rp2040-zero/
target_compile_definitions(stub PRIVATE
  PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

target_include_directories(stub 
   PRIVATE include
)

target_link_libraries(
    stub 

    pico_stdlib

    hardware_pio 
)
