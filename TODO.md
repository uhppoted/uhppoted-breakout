# TODO

- [ ] pico: txrx/uart conflict
- [ ] debounce simulation
- [ ] controller ID
      - https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#rpip2aed586200427c10f67f
      - compile time constant

### SMP

### CLI
    - [ ] command line
          - [x] ANSI/VT100 codes
          - [x] set scroll area
          - [x] print command results to command area
          - [x] help panel
          - [ ] initialise on connected
                - [ ] // FIXME remove - delay to let USB initialise
          - [ ] set initial scroll area
          - [ ] reset command timer on rx char
          - [ ] timeout is weird
    - [ ] Dangling '['
    - https://hackaday.com/2024/04/17/human-interfacing-devices-hid-over-i2c

### U5 RX8900SA
    - [x] Print date/time on ready
    - [ ] initialise on-chip RTC from RX8900SA

### log
   - [x] Queue log writes
   - [ ] MSG_LOG
   - [ ] Pop first log entry on full
   - [ ] Print '...' after queue full
   - [ ] Partially printing in tight loop (check return value of printf ?)
   - [ ] Wait for USB connected
               
## UART
   - [ ] usb_init
   - [ ] Enable FIFO
   - [ ] // FIXME use struct with length (or match on SOM/EOM or something)
   - https://forums.raspberrypi.com/viewtopic.php?t=327189
   - https://stackoverflow.com/questions/76367736/uart-tx-produce-endless-interrupts-how-to-acknowlage-the-interrupt
   - https://www.baeldung.com/cs/bisync
   - https://stackoverflow.com/questions/51609813/serial-communication-in-c-for-raspberry-pi
   - https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
   - https://tldp.org/HOWTO/Serial-Programming-HOWTO/x56.html

## PiZeroW
   - [ ] Reduce power consumption
   - [ ] NB: 24/7 operation - Cam said to ask him

### OTA
   - https://blog.usedbytes.com/2021/12/pico-serial-bootloader/
   - https://www.youtube.com/watch?v=4-dle5L9REs
   - https://monocypher.org

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


