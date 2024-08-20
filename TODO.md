# TODO

- [ ] debounce simulation
      - (?) TinyML
- [ ] https://interrupt.memfault.com/blog/diving-into-jtag-part-6

### emulator
    - [ ] get-controller
          - [x] from MIB
          - [ ] errors pipe in request
    - [x] move encoding to encoding/BER
    - [x] move encoding to encoding/bisync
    - [x] decode - DLE 
    - [x] require STX-ETX framing
    - [x] limit packet size
    - [x] use SNMP request/message ID
    - [x] ENQ/ACK loop - use codec
          - [x] Use callback to handle decoded messages
          - (?) use common rx codec??
          - (?) use callback funcs rather than interface
    - [ ] CRC
    - [ ] Fix 'read' goroutine that never exits
    - (?) decode - partial packets
    - [ ] prune handlers
    - [ ] idle poll
    - [ ] walk MIB to initialise/refresh cache
    - [ ] // TODO: exponential backoff
    - [ ] tinygo


### SSMP
   - [x] disable print/println in SSMP mode
   - [x] SYN-SYN-ENQ/SYN-SYN-ACK
   - [ ] MIB
   - [ ] GET
         - [x] // FIXME doesn't work with binary protocol
         - [x] decode::DLE 
         - [x] return SnmpGetResponse
         - [x] encode::DLE 
         - [x] require STX-ETX framing
         - [ ] CRC
         - [ ] BER
               - [ ] not unpacking multiple fields with different lengths correctly
               - [ ] commonalise decode length
               - https://www.oss.com/asn1/resources/asn1-made-simple/asn1-quick-reference/basic-encoding-rules.html
```
seq   48
      37
      2  1 0 4 6 112 117 98 108 105 99 160 24 2  1   1
      2  1 0 2 1 0   48  13 48  11  6  7   43 6  167 254
      32 1 1 5 0

int 2 1 0

```
   - [x] SSMP idle - revert to MODE_UNKNOWN
   - [ ] Enable FIFO
   - [ ] pico: txrx/uart conflict
   - https://www.baeldung.com/cs/bisync
   - http://www.tcpipguide.com/free/t_SNMPVersion2SNMPv2MessageFormats-5.htm
   - https://www.ranecommercial.com/legacy/note161.html
   - [PPP](https://datatracker.ietf.org/doc/html/rfc1661)
   - https://people.ece.cornell.edu/land/courses/ece4760/RP2040/C_SDK_random/index_random.html
   - https://forums.raspberrypi.com/viewtopic.php?t=302960

### MIB
    - [ ] controller ID
          - https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#rpip2aed586200427c10f67f
          - compile time constant

### CLI
    - [x] initialise on connected
    - [x] set initial scroll area
    - [x] Dangling '['
    - [x] // FIXME remove - delay to let USB initialise
    - [x] LED yellow/green for UNKNOWN mode
    - [x] LED red/green for CLI mode
    - [x] LED blue/green for SSMP mode
    - [x] CLI idle - ping for terminal status
    - [x] parse buffer with multiple terminal report messages
    - [x] echo weirdness: printf works but sys::print doesn't
    - [ ] parse commands using strtok

## PiZeroW
   - [ ] Reduce power consumption
   - [ ] NB: 24/7 operation - Cam said to ask him
   - (?) Use Nerves

### OTA
   - https://blog.usedbytes.com/2021/12/pico-serial-bootloader/
   - https://www.youtube.com/watch?v=4-dle5L9REs
   - https://monocypher.org
   - https://groupgets.com/products/pureconnect

## R&D
- [x] Relay pin voltages
- [x] CPU board power consumption
- [x] Breakout board power consumption
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


