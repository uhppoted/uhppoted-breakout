# TODO

## RPCD
   - [x] commonalise GET/SET queue handling
   - [ ] daemonize
   - [ ] monitor
   - (?) Context.WithTimeout 
   - (?) https://go.dev/blog/synctest
   - socat: https://tinkering.xyz/async-serial/

## breakout

### misc

- [ ] PIO I2C
- [ ] make debugf, infof, etc interrupt safe
   - https://embeddedartistry.com/blog/2019/11/06/an-embedded-friendly-printf-implementation
   - https://github.com/LukeShu/pico-fmt

- [ ] USB
    - [ ] picotool reset
        - https://forums.raspberrypi.com/viewtopic.php?t=366208
        - https://github.com/raspberrypi/picotool/issues/88
        - https://github.com/raspberrypi/picotool/pull/83
        - https://github.com/earlephilhower/arduino-pico/issues/2078
        - https://github.com/raspberrypi/picotool/blob/f6fe6b7c321a2def8950d2a440335dfba19e2eab/picoboot_connection/picoboot_connection.c#L58-L82
        - https://github.com/raspberrypi/picotool/pull/177
        - https://github.com/illusiaDecafish/bootselBoot
        - https://github.com/raspberrypi/pico-sdk/blob/master/src/rp2_common/pico_stdio_usb/reset_interface.c
        - https://github.com/ithinuel/usbd-picotool-reset
        - https://github.com/dsi-modchip/firmware
        - https://github.com/tomas-pecserke/pico_tusb_reset_interface
        - https://github.com/hathach/tinyusb/discussions/664
        - https://github.com/piersfinlayson/tinyusb-vendor-example

- [ ] Rework print queue
      - (?) reservoir sampling (https://samwho.dev/reservoir-sampling)

### SSMP
   - [ ] TRAP
      - [x] inputs
      - [ ] power on/reset
         - (?) queue
      - [ ] swipe
      - [ ] alarm
      - [ ] //FIXME encode all var types
      - (?) change to INFORM

   - [ ] save to flash
      - [ ] door delay/mode

   - [ ] `DEBUG  DOORS  settings saved` on `set-time`
      - only save if OID in 'save list'
      - // FIXME check OID save list

   - [ ] GET: multiple OIDs
   - [ ] MIB_find (returns const *MIBItem)
   - [ ] Use 'perfect' hash function
      - ref. https://www.gnu.org/software/gperf

### ACL
- [ ] open door on PB
- [ ] open door on card swipe

## Zero2W
- [ ] serial logger
- [ ] custom image
      - https://github.com/raspberrypi/rpi-image-gen
- [ ] memory FS for Unix domanin sockets
      - /run
      - /dev/shm
      - tmpfs

### UT0311
   - [ ] eventd
      - [x] add
      - [x] get
      - [ ] remove unnecessary OID stuff
      - [ ] move entities to shared lib

   - [x] store events to SQLite DB
   - [x] rpcd::shutdown RPC listener
   - [ ] open door on PB
   - [ ] open door on card swipe
   - [ ] rework with text tags

   - [ ] API
      - [x] get-controller
      - [x] set-address
      - [x] get-listener
      - [x] set-listener
      - [x] get-time
      - [x] set-time
      - [x] get-status
      - [x] get-door
      - [x] set-door
      - [x] set-door-passcodes
      - [x] get-cards
      - [x] get-card
      - [x] get-card-by-index
      - [x] put-card
      - [x] delete-card
      - [x] delete-all-cards
      - [x] open-door
      - [x] get-event-index
      - [x] set-event-index
      - [x] get-event
      - [x] record-special-events
      - [ ] get-time-profiles
      - [ ] set-time-profile
      - [ ] clear-time-profiles
      - [ ] add-task
      - [ ] clear-task-list
      - [ ] refresh-task-list
      - [ ] set-pc-control
      - [ ] set-interlock
      - [ ] activate-keypads
      - [ ] get-anti-passback
      - [ ] set-anti-passback
      - [ ] restore-default-parameters

   - [ ] events
      - [ ] forced open

   - [ ] RPC
      - https://go.dev/blog/protobuf-opaque
      - [x] go-rpc
      - [x] unix domain sockets
      - [ ] go-rpc+json
      - [ ] TLS
      - [ ] TCP/IP
      - [ ] grpc

   - [ ] monitor
   - [ ] daemonize
   - [ ] idle poll
   - [ ] cache
   - [ ] tinygo
      - https://di-marco.net/blog/it/2020-06-06-raspberry_pi_3_4_and_0_w_serial_port_usage/

### MIB
   - [ ] RP2040 ID
      - https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#rpip2aed586200427c10f67f
   - [ ] system errors
   - [ ] system error messages
   - [ ] use DATETIME, BOOLEAN, ENUMERATED etc

## PiZeroW
   - [ ] Reduce power consumption
      - https://hackaday.com/2024/10/03/pi-zero-power-optimization-leaves-no-stone-unturned
      - https://kittenlabs.de/blog/2024/09/01/extreme-pi-boot-optimization
   - [ ] NB: 24/7 operation - Cam said to ask him
   - (?) Use Nerves
   - https://hackaday.com/2025/03/26/build-customized-raspberry-pi-os-images-with-rpi-image-gen/

### OTA
   - https://blog.usedbytes.com/2021/12/pico-serial-bootloader/
   - https://www.youtube.com/watch?v=4-dle5L9REs
   - https://monocypher.org
   - https://groupgets.com/products/pureconnect

## R&D
- Nerves
- Embassy (Rust)
- https://interrupt.memfault.com/blog/diving-into-jtag-part-6
- https://pwy.io/posts/marching-events/
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
   - (?) https://github.com/splitbrain/pinoutleaf
- OCDSP
   - https://github.com/bytedreamer/Aporta
- Mongoose Wizard
   - https://mongoose.ws/wizard/#/

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
11. https://hackaday.com/2024/05/04/giving-your-kicad-pcb-repository-pretty-pictures/
12. External flash
   - https://mcuoneclipse.com/2022/12/04/add-extra-storage-to-the-raspberry-pi-pico-with-w25q128-and-littlefs
13. https://forums.raspberrypi.com/viewtopic.php?t=327189
14. https://hackaday.com/2024/04/17/human-interfacing-devices-hid-over-i2c
15, https://github.com/raspberrypi/pico-sdk/issues/224
16. https://github.com/raspberrypi/pico-sdk/issues/997
