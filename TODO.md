# TODO

## Rev.0
   - [x] Fix schematic for RP2040 Tiny (https://www.waveshare.com/rp2040-tiny.htm)
   - [x] mouser.ca
   - [x] Fix RP2040Z symbol + footprint
   - [x] Fix RX8900SA footprint
   - [x] README images
   - [x] BOM
   - [ ] Bring-up checklist

### log
               
### CLI
    - [x] debug
    - [x] get/set-time
    - [x] get/set-date
    - [ ] get/set-datetime
    - [ ] get/set-weekday
    - [x] reset RTC

### U2 PCAL6408APW
    - [ ] init

### RX8900SA
    - [ ] Defer setup with alarm timer
    - [x] Rework setup to initialise registers as a block
    - [x] get/set-time
    - [x] get/set-date
          - Zeller's congruence for weekday
    - [ ] get/set-datetime
    - [x] reset
    - [x] initialise
    - [ ] ready
    - [x] wrap in RTC.c
    - [ ] handle initialisation sequence in RTC_init
    - [ ] backup mode compile flags

## R&D
- [ ] Document pin voltage levels
      - [x] Relays

- [ ] Annotated photos
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
