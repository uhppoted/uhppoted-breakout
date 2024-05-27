cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(breakout C CXX ASM)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CORE "../core")
set(LIB  "../lib")

pico_sdk_init()

add_executable(breakout
  src/main.c
  src/sys.c
  src/uart.c
  src/cli.c
  src/hwconfig.c
  src/I2C0.c

  ${CORE}/src/I2C1.c
  ${CORE}/src/RTC.c
  ${CORE}/src/U2.c
  ${CORE}/src/IOX.c
  ${CORE}/src/readers.c
  ${CORE}/src/log.c

  ${LIB}/src/I2C.c
  ${LIB}/src/RX8900SA.c
  ${LIB}/src/PCAL6408APW.c
)

target_include_directories(breakout 
   PRIVATE include
   PRIVATE ${CORE}/include
   PRIVATE ${LIB}/include
)

pico_set_program_name(breakout "uhppoted-breakout")
pico_set_program_version(breakout "0.0")

pico_enable_stdio_usb (breakout 0)
pico_enable_stdio_uart(breakout 1)
pico_add_extra_outputs(breakout)

target_link_libraries(
    breakout 
    pico_stdlib
    hardware_i2c
)
