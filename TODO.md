# TODO

- [ ] IPv4 address/netmask/gateway
      - [ ] configure interface
      - [ ] configure from .config
      - (?) store on breakout

### SSMP
    - [x] set USB output translation
    - [x] GET
    - [x] HOTP
    - [ ] ENQ/ACK loop
    ```
    request error (RX queue blocked)
    request error (TX queue blocked)
     ```
    - [ ] ASN.1
       - [x] Fix negative int encoding
       - [x] Fix integer decoding
       - [x] free_packet: dynamic
       - [ ] check slice_free
       - [ ] check field_free
       - [ ] check vector_free
       - [ ] https://forums.raspberrypi.com/viewtopic.php?t=347638 
    
    - [ ] UART
        - [ ] set UART output translation (`uart_set_translate_crlf`)
        - [ ] Enable FIFO
        - [ ] pico: txrx/uart conflict
        - [PPP](https://datatracker.ietf.org/doc/html/rfc1661)

### emulator
    - [ ] ASN.1
    - [ ] log CRC errors
    - [ ] get-controller
          - [ ] MAC
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
    - [ ] tinygo
    - [ ] python

### MIB
    - [x] controller ID
    - [ ] controller address
          - https://github.com/jackpal/gateway
    - [ ] RP2040 ID
          - https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#rpip2aed586200427c10f67f

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
14. https://stackoverflow.com/questions/76367736/uart-tx-produce-endless-interrupts-how-to-acknowlage-the-interrupt
15. https://www.baeldung.com/cs/bisync
16. https://stackoverflow.com/questions/51609813/serial-communication-in-c-for-raspberry-pi
17. https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
18. https://tldp.org/HOWTO/Serial-Programming-HOWTO/x56.html
19. https://hackaday.com/2024/04/17/human-interfacing-devices-hid-over-i2c
20. https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum
21, https://github.com/raspberrypi/pico-sdk/issues/224
22. https://github.com/raspberrypi/pico-sdk/issues/997
