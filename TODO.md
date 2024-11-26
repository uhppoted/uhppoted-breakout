# TODO

- [x] Make Rev.0 emulator a 'local only' package
- [ ] Zero2W pigpio UART
- [ ] Rework print queue
      - [x] use circular buffer of char[128]
      - [ ] remove free(msg) from _push
      - [ ] overflow "..."
      - [ ] use timer + MSG_LOG

- [ ] set I2C GPIO slew rates and drive for 100kHz
- [ ] Commonalise sys implementation into core::sys

- [ ] CLI: trace interval
- [ ] CLI: poke
      - system errors
      - system error messages

- [ ] syserror
      - (?) count up/down to auto-clear
      - (?) orange LED on any system error 
      - (?) log once on occurrence
      - (?) display in 'poke'

- [ ] Use Pico optimized libraries
      - pico_mem_ops
      - pico_printf
      - pico_malloc
      - pico_stdio

### SSMP
    - [ ] Migrate SSMP handler to UART1
          - [x] ENQ
          - [x] GET
          - [x] pico: txrx/uart conflict
          - [x] Remove MODE
          - [x] Remove SSMP stuff from CLI
          - [x] Move USB connected/disconnected to USB driver
          - [x] pass buffer address in MSG
          - [x] Move query-status + mode to sys
          - [x] MODE_UNKNOWN on startup and then wait for query status response
          - [x] MODE_CLI timeout
          - [ ] weird intermittent reset
                - ~~too many get requests~~
                - ~~full queue~~
                - ~~bisync decoder~~
                - ~~UART interrupt handler~~
                - ~~IOX or RTC~~
                - ~~USB~~
                - ~~loop in _print~~
                - **CLI TERMINAL_QUERY_STATUS printf**
                - something in sequence ? 
                - U3_read ?
                - alarms/timers disabled !!!!
                - check all mutexes are try-lock (avoid priority inversion) 
                - weird SSMP response but message processing is commented out????? And approximately when board did its weird thing
```
... debug     264 16 16 02 30 0e 10 06 0a 2b 10 ....
... received 16 16 02 30 0e 10 06 0a
... received 2b 10 06 10 01 04 10 01
... received 84 80 00 10 02 10 01 10
... received 05 00 03 d3 89
... debug     265 16 16 02 30 0e 10 06 0a 2b 10 ....
... received 16 16 02 30 0e 10 06 0a
```

          - [ ] Check all FIXMEs
          - [ ] Invoke sys.flush from main loop rather than timer handler
          - [ ] Check that system doesn't freeze when print queue is full 
                (i.e. why did it freeze when the CLI used a printf?)
          - [ ] Maybe only enable SSMP interrupt after a delay? 
          - [ ] bisync max message size
          - (?) bisync timeout
          - [ ] Reinstate watchdog reset

    - monitor
      - get-board

    - [ ] memory leak

    - get-controller
      - [x] version
      - [ ] released
      - [ ] cache

    - [ ] ASN.1
       - [x] Fix negative int encoding
       - [x] Fix integer decoding
       - [x] free_packet: dynamic
       - [ ] check slice_free
       - [ ] check field_free
       - [ ] check vector_free
       - (?) free_value
       - [ ] https://forums.raspberrypi.com/viewtopic.php?t=347638 
    
    - [ ] UART
        - [x] set UART output translation (`uart_set_translate_crlf`)
        - [x] Enable FIFO

### emulator
    - [ ] split architecture
          - SSMP driver
          - UHPPOTE emulator
          - RPC
            - unix domain sockets
            - TCP/IP

    - [ ] python
          - [ ] aioconsole

    - [ ] tinygo
          - https://di-marco.net/blog/it/2020-06-06-raspberry_pi_3_4_and_0_w_serial_port_usage/

    - [ ] ENQ/ACK loop
    ```
    request error (RX queue blocked)
    request error (TX queue blocked)
     ```
    - [ ] ASN.1
    - [ ] log CRC errors
    - [ ] get-controller
          - [ ] errors pipe in request
          - [ ] serialize requests

    - [x] ENQ/ACK loop - use codec
          - (?) use common rx codec??
          - (?) use callback funcs rather than interface
    - [ ] Fix 'read' goroutine that never exits
    - (?) decode - partial packets
    - [ ] prune handlers
    - [ ] idle poll
    - [ ] walk MIB to initialise/refresh cache
    - [ ] // TODO: exponential backoff

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
- [ ] https://interrupt.memfault.com/blog/diving-into-jtag-part-6
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
