# TODO

- [x] Power on/reset and reprogram initial states are unexpectedly different
- [x] Rev 0.1
      - [x] RTC Schottky diodes and resistor

- [ ] `I2C0 queue full` on power on
       - maybe USB char?
       - **BECAUSE** repeating timers kick in before system initialisation is complete and I2C0 conflicts
```
ERROR  I2C0       push: queue full
ERROR  U3         update: queue full
...
```

- [ ] pico: txrx/uart conflict
- [ ] debounce simulation

### CLI
    - [ ] reboot
    - [ ] render state
    - (?) command line
    - (?) UI thread
    - [ ] Dangling '['
    - https://hackaday.com/2024/04/17/human-interfacing-devices-hid-over-i2c

### U2 PCAL6408APW
    - [ ] Decode keypad
    - [ ] Read timeout
    - (?) Continuous read 
          - Use I2C1 to check last register (a la I2C0)
          - Request with address after time delay, continuous otherwise
    - (?) 100kHz/400kHz compile flag

### U3 PCAL6408APW
   - [ ] Error if input is indeterminate (i.e. chattering)

### U4 PI4IOE5V6416
    - [x] rework as clocked update
    - [x] rework relay set/clear as ticked operations
    - [x] rework LED set/clear as ticked operations
    - [x] rework SYS,IN, ERR as ticked operations
    - [x] inline (https://stackoverflow.com/questions/31108159/what-is-the-use-of-the-inline-keyword-in-c)
    - [x] sanity check PI4IOE5V6416 state against U4x.outputs
    - [x] invert SYS, IN and ERR LEDs
    - [x] Initial readback always reads back 0x0000
    - [x] check polarity
          - [x] RELAY 1
          - [x] RELAY 2
          - [x] RELAY 3
          - [x] RELAY 4
          - [x] LED 1
          - [x] LED 2
          - [x] LED 3
          - [x] LED 4
          - [x] SYS
          - [x] ERR
          - [x] IN
    - [ ] Set output drive

### RTC/RX8900SA
    - [ ] Defer setup with alarm timer
    - [ ] ready
    - [ ] handle initialisation sequence in RTC_init
    - [ ] backup mode compile flags

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

## R&D
- [x] Relay pin voltages
- [ ] CPU board power consumption
- [ ] Breakout board power consumption

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

## KiCad schematic
- https://www.flux.ai/p


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
