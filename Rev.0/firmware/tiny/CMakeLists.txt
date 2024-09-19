cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(breakout C CXX ASM)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_compile_definitions(RX8900SA_VDET="${RX8900SA_VDET}")
add_compile_definitions(RX8900SA_SWOFF="${RX8900SA_SWOFF}")
add_compile_definitions(RX8900SA_BKSMP="${RX8900SA_BKSMP}")
add_compile_definitions(I2C0_CLOCK="${I2C0_CLOCK}")
add_compile_definitions(I2C1_CLOCK="${I2C1_CLOCK}")
add_compile_definitions(CONTROLLER=${CONTROLLER})

set(CORE "../core")
set(LIB  "../lib")

pico_sdk_init()

add_executable(breakout
  src/main.c
  src/sys.c
  src/hwconfig.c

  ${CORE}/src/sys.c
  ${CORE}/src/I2C0.c
  ${CORE}/src/I2C1.c
  ${CORE}/src/RTC.c
  ${CORE}/src/IOX.c
  ${CORE}/src/U2.c
  ${CORE}/src/U3.c
  ${CORE}/src/U4.c
  ${CORE}/src/SSMP.c
  ${CORE}/src/MIB.c
  ${CORE}/src/auth.c
  ${CORE}/src/state.c
  ${CORE}/src/log.c
  ${CORE}/src/cli.c
  ${CORE}/src/ANSI.c

  ${CORE}/src/usb.c

  ${LIB}/src/I2C.c
  ${LIB}/src/RX8900SA.c
  ${LIB}/src/PCAL6408A.c
  ${LIB}/src/PCAL6416A.c

  ${LIB}/src/types/slice.c
  ${LIB}/src/encoding/bisync/bisync.c
  ${LIB}/src/encoding/bisync/crc.c
  ${LIB}/src/encoding/ASN.1/encode.c
  ${LIB}/src/encoding/ASN.1/decode.c
  ${LIB}/src/encoding/ASN.1/fields.c
  ${LIB}/src/encoding/ssmp/ssmp.c
  ${LIB}/src/encoding/ssmp/get.c
  ${LIB}/src/encoding/ssmp/get_response.c
  ${LIB}/src/auth/hotp/hotp.c
  ${LIB}/src/crypt/hash/sha256.c
)

target_include_directories(breakout 
   PRIVATE ${CORE}/include
   PRIVATE ${LIB}/include
)

pico_generate_pio_header(
    breakout
    ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio
)

pico_set_program_name(breakout "uhppoted-breakout")
pico_set_program_version(breakout "0.0")

pico_enable_stdio_usb (breakout 1)
pico_enable_stdio_uart(breakout 0)
pico_add_extra_outputs(breakout)

target_link_libraries(
    breakout 

    pico_stdlib
    pico_multicore
    pico_sync

    hardware_i2c
    hardware_pio 
    hardware_uart
    hardware_watchdog
    hardware_rtc
)
