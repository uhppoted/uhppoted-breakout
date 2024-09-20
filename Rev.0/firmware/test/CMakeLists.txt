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
  src/hwconfig.c

  src/crypt/hash/sha1_test.c
  src/crypt/hash/sha256_test.c
  src/encoding/ASN.1/BER_encode_test.c
  src/encoding/ASN.1/BER_decode_test.c

  ${LIB}/src/crypt/hash/sha1.c
  ${LIB}/src/crypt/hash/sha256.c
  ${LIB}/src/encoding/ASN.1/encode.c
  ${LIB}/src/encoding/ASN.1/decode.c
  ${LIB}/src/encoding/ASN.1/fields.c
  ${LIB}/src/types/slice.c
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

    hardware_pio 
    hardware_uart
    hardware_watchdog
    hardware_rtc
)
