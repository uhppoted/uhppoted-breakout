# TODO

- [x] Make Rev.0 emulator a 'local only' package
- [ ] Zero2W serial logging
- [ ] Zero2W USB UART
- [ ] MSG_POKE (because absolutely shouldn't call it from interrupt handler)
- [ ] Rework print queue
      - (?) check USB connected
      - (?) setvbuf

- [ ] set I2C GPIO slew rates and drive for 100kHz
- [ ] Commonalise sys implementation into core::sys
- (?) mutex around buffer push/pop
      - requires 'double' buffering otherwise pop will block interrupt reads
      - or use queue ?

- [ ] CLI: trace interval
- [ ] CLI: poke
      - system errors
      - system error messages

- [ ] syserror
      - (?) count up/down to auto-clear
      - (?) red LED on any system error 
      - (?) log once on occurrence
      - (?) display in 'poke'

- [ ] system monitor
      - [x] error on e.g. 50% usage
      - [ ] check memory usage monitor

- [ ] Use Pico optimized libraries
      - pico_mem_ops
      - pico_printf
      - pico_malloc
      - pico_stdio

### SSMP
    - GET
      - [x] controller ID
      - [x] version
      - [ ] released

    - [ ] ASN.1
       - [x] packet_free: dynamic
       - [x] vector_free
       - [ ] check field_free
       - [ ] check slice_free
       - (?) field pool
    
### emulator
    - [x] write config to tmp file and move 
    - [x] get-controller
    - [x] set-address
    - [x] get-listener
    - [x] set-listener
    - [x] get-time
    - [x] set-time
    - [ ] get-status

    - [ ] API
           - [x] UDP
           - [x] TCP/IP
                 - [ ] allow multiple requests
           - [ ] TLS
                 - [x] CLI TLS
                 - [x] mutual auth
                 - [ ] configuration
                 - [ ] allow multiple requests
           - [ ] Python CLI TCP socket not terminating/closing if no reply
           - [ ] Terminate gracefully on CTRL-C
           - [ ] exponential backoff

    - [ ] RPC
          - [x] go-rpc
          - [ ] grpc
          - [ ] unix domain sockets
          - [ ] TCP/IP

    - [ ] monitor
    - [ ] daemonize
    - [ ] idle poll
    - [ ] cache
    - [ ] tinygo
          - https://di-marco.net/blog/it/2020-06-06-raspberry_pi_3_4_and_0_w_serial_port_usage/
    - [ ] Nerves

### Driver
    - [ ] breakout-simulator
    - [ ] cache controller ID

### CLI
    - Update on uhppoted-python release 0.8.10
      - [ ] get-listener
      - [ ] set-listener

### MIB
    - [x] controller ID
    - [ ] RP2040 ID
          - https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#rpip2aed586200427c10f67f
    - [ ] system errors
    - [ ] system error messages

## PiZeroW
    - [ ] Reduce power consumption
          - https://hackaday.com/2024/10/03/pi-zero-power-optimization-leaves-no-stone-unturned
          - https://kittenlabs.de/blog/2024/09/01/extreme-pi-boot-optimization
    - [ ] NB: 24/7 operation - Cam said to ask him
    - (?) Use Nerves

### OTA
    - https://blog.usedbytes.com/2021/12/pico-serial-bootloader/
    - https://www.youtube.com/watch?v=4-dle5L9REs
    - https://monocypher.org
    - https://groupgets.com/products/pureconnect

## R&D
- https://interrupt.memfault.com/blog/diving-into-jtag-part-6
- [ ] Pinout diagram
     - (?) [WireViz] https://github.com/formatc1702/WireViz
     - (?) https://github.com/cmfcmf/ic-pinout-diagram-generator
     - (?) https://pinouts.vercel.app/board/nodemcu-v2
     - (?) https://electronics.stackexchange.com/questions/73780/program-to-draw-pinouts-and-easily-visualize-pins-multiplexed-functions
     - (?) https://www.youtube.com/watch?v=ndVs1UvK6AE
     - (?) TeX
     - (?) https://www.sparkfun.com/news/1947
     - (?) https://www.flickr.com/photos/28521811@N04/
     - (?) https://github.com/stevenj/GenPinoutSVG
     - (?) https://yaqwsx.github.io/Pinion/

### Rust
- https://rtic.rs/1/book/en/preface.html
- https://github.com/embassy-rs/embassy

## Notes
1. https://github.com/yaqwsx/PcbDraw
2. https://yaqwsx.github.io/Pinion/
3. https://en.wikipedia.org/wiki/Simple_Service_Discovery_Protocol
4. https://en.wikipedia.org/wiki/Zero-configuration_networking
5. https://learn.sparkfun.com/tutorials/micromod-rp2040-processor-board-hookup-guide/all#introduction
6. https://www.proto-advantage.com/store/product_info.php?products_id=2200040
7. I2C pullups
      - https://electronics.stackexchange.com/questions/1849/is-there-a-correct-resistance-value-for-i2c-pull-up-resistors
      - https://www.ti.com/lit/an/slva689/slva689.pdf
      - https://www.edn.com/design-calculations-for-robust-i2c-communications/
8. https://www.cnx-software.com/2021/12/09/raspberry-pi-zero-2-w-power-consumption
9. https://raspi.tv/2017/how-much-power-does-pi-zero-w-use
10. DC-DC converter modules:
    - https://www.sparkfun.com/products/18375
    - https://www.adafruit.com/product/1065
    - https://www.adafruit.com/product/4739
    - https://www.adafruit.com/product/1385
11. https://hackaday.com/2024/05/04/giving-your-kicad-pcb-repository-pretty-pictures/
12. External flash
    - https://mcuoneclipse.com/2022/12/04/add-extra-storage-to-the-raspberry-pi-pico-with-w25q128-and-littlefs
13. https://forums.raspberrypi.com/viewtopic.php?t=327189
14. https://hackaday.com/2024/04/17/human-interfacing-devices-hid-over-i2c
15, https://github.com/raspberrypi/pico-sdk/issues/224
16. https://github.com/raspberrypi/pico-sdk/issues/997
