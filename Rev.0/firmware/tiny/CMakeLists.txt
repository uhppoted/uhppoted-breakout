cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(breakout C CXX ASM)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# set(CORE "../../../core")
# set(COMMON "../../common")

pico_sdk_init()

add_executable(breakout
  src/main.c
)

target_include_directories(breakout PRIVATE ${CORE}/include)

pico_generate_pio_header(
    breakout
    ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio
)

pico_set_program_name(breakout "uhppoted-breakout")
pico_set_program_version(breakout "0.0")

pico_enable_stdio_usb (breakout 0)
pico_enable_stdio_uart(breakout 1)
pico_add_extra_outputs(breakout)

target_link_libraries(
    breakout 
    pico_stdlib
    hardware_pio 
)
