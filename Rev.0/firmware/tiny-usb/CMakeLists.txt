cmake_minimum_required(VERSION 3.5)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(breakout C CXX ASM)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

add_compile_definitions(PICO_ENTER_USB_BOOT_ON_EXIT=1)
add_compile_definitions(PARAM_ASSERTIONS_ENABLE_ALL=1)

add_compile_definitions(RX8900SA_VDET="${RX8900SA_VDET}")
add_compile_definitions(RX8900SA_SWOFF="${RX8900SA_SWOFF}")
add_compile_definitions(RX8900SA_BKSMP="${RX8900SA_BKSMP}")
add_compile_definitions(I2C0_CLOCK="${I2C0_CLOCK}")
add_compile_definitions(I2C1_CLOCK="${I2C1_CLOCK}")
add_compile_definitions(WATCHDOG="${WATCHDOG}")
add_compile_definitions(MODE="${MODE}")
add_compile_definitions(SSMP="${SSMP}")

add_compile_definitions(CONTROLLER=${CONTROLLER})
add_compile_definitions(VERSION=${VERSION})
add_compile_definitions(RELEASED="${RELEASED}")
add_compile_definitions(TRACE=${TRACE})

pico_sdk_init()

set(TARGET_NAME "breakout")
set(CORE "../core")
set(LIB  "../lib")

add_executable(${TARGET_NAME}
  src/main.c
  src/sys.c
  src/hwconfig.c
  src/usb_descriptors.c

  ${CORE}/src/breakout.c
  ${CORE}/src/sys.c
  ${CORE}/src/log.c
  ${CORE}/src/I2C0.c
  ${CORE}/src/I2C1.c
  ${CORE}/src/RTC.c
  ${CORE}/src/IOX.c
  ${CORE}/src/U2.c
  ${CORE}/src/U3.c
  ${CORE}/src/U4.c
  ${CORE}/src/settings.c
  ${CORE}/src/doors.c
  ${CORE}/src/auth.c
  ${CORE}/src/SSMP.c
  ${CORE}/src/cli.c
  ${CORE}/src/ANSI.c
  ${CORE}/src/mempool.c
  ${CORE}/src/MIB/MIB.c
  ${CORE}/src/MIB/find.c
  ${CORE}/src/MIB/get.c
  ${CORE}/src/MIB/set.c
  ${CORE}/src/usb.c

  ${LIB}/src/I2C.c
  ${LIB}/src/RX8900SA.c
  ${LIB}/src/PCAL6408A.c
  ${LIB}/src/PCAL6416A.c

  ${LIB}/src/types/slice.c
  ${LIB}/src/types/packet.c
  ${LIB}/src/types/fields.c
  ${LIB}/src/types/buffer.c

  ${LIB}/src/encoding/bisync/bisync.c
  ${LIB}/src/encoding/bisync/crc.c
  ${LIB}/src/encoding/ASN.1/encode.c
  ${LIB}/src/encoding/ASN.1/decode.c
  ${LIB}/src/encoding/ssmp/ssmp.c
  ${LIB}/src/encoding/ssmp/get.c
  ${LIB}/src/encoding/ssmp/set.c
  ${LIB}/src/encoding/ssmp/response.c
  ${LIB}/src/encoding/ssmp/trap.c
  ${LIB}/src/crypt/hash/djb2.c
  ${LIB}/src/crypt/hash/sha1.c
  ${LIB}/src/crypt/hmac/hmac_sha1.c
  ${LIB}/src/crypt/otp/hotp.c
)

pico_generate_pio_header(
    ${TARGET_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio
)

pico_set_program_name(${TARGET_NAME} "uhppoted-breakout-usb")
pico_set_program_version(breakout "0.0")

pico_enable_stdio_usb (breakout 1)
pico_enable_stdio_uart(breakout 0)
pico_add_extra_outputs(breakout)

# Ref. https://tinygo.org/docs/reference/microcontrollers/machine/waveshare-rp2040-zero/
target_compile_definitions(${TARGET_NAME} PRIVATE
  PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

target_include_directories(${TARGET_NAME} 
   PRIVATE include
   PRIVATE ${CORE}/include
   PRIVATE ${LIB}/include
)

target_link_libraries(
    ${TARGET_NAME} 

    pico_stdlib
    pico_multicore
    pico_sync
    pico_unique_id

    hardware_i2c
    hardware_pio 
    hardware_watchdog
    hardware_flash

    tinyusb_device
    tinyusb_board
)
