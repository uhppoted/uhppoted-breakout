# NOTES

1.  https://www.digikey.ca/en/articles/use-tight-pitch-board-to-board-connectors
2.  https://github.com/vlang/vinix
3.  BSS1338 or 2N7002 https://hackaday.com/2023/05/02/fet-fun-endeavors-together
4.  https://hackaday.com/2016/12/05/taking-it-to-another-level-making-3-3v-and-5v-logic-communicate-with-level-shifters/
5.  https://www.cnx-software.com/2023/10/09/waveshare-rp2040-pizero-raspberry-pi-rp2040-board-raspberry-pi-zero-form-factor/
6.  https://hackaday.com/2024/03/18/rnode-a-portable-unrestricted-digital-radio
7.  https://www.diodes.com/design/support/technical-articles/simplify-your-embedded-designs-using-ic-io-expanders
8.  https://www.diodes.com/assets/Datasheets/PI4IOE5V6416.pdf
9.  https://www.diodes.com/assets/Datasheets/PI4IOE5V6524.pdf
10. SMD adapters
    - https://www.icbreakout.com/products/qfn-24-breakout-board-4x4-mm-0-5-mm
    - https://www.artekit.eu/products/breakout-boards/bbadapters/tssop-to-dip/da-tssop24-p65
    - https://www.mikroe.com/tssop-ssop-24-to-dip24-300mil-adapter
    - https://www.aliexpress.com/item/1195743182.htm
11. https://electronics.stackexchange.com/questions/1849/is-there-a-correct-resistance-value-for-i2c-pull-up-resistors
12. https://learn.sparkfun.com/tutorials/micromod-rp2040-processor-board-hookup-guide/all#introduction


13. https://pmdway.com/products/soic24-tssop24-to-dip-adaptor-pcbs-10-pack
14. https://www.aliexpress.com/item/2039729656.html
15. https://www.mouser.ca/ProductDetail/TE-Connectivity/2-2316108-0
16. https://www.te.com/usa-en/product-2-2316108-0.html
17. https://electronics.stackexchange.com/questions/327455/multi-byte-i2c-read-not-working-while-equivalent-arduino-one-does

18. https://nats.io
19. https://store.particle.io/collections/all-products/products/photon-2
20. https://youtu.be/RrUyS-gixWk?si=QhU_jPk8qZ5NfyuA

## RX8900SA
1. https://github.com/citriena/RX8900RTC/blob/master/src/RX8900RTC.cpp

## SPI
   - https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/all#spi-on-pi
   - https://embetronicx.com/tutorials/linux/device-drivers/linux-kernel-spi-device-driver-tutorial
   - https://github.com/cpb-/spi-tools
   - https://github.com/joan2937/picod/blob/master/DAEMON/picod.c
   - https://raspberrypi.stackexchange.com/questions/138460/software-spi-interrupt
   - https://raspberrypi.stackexchange.com/questions/140153/pi-pico-spi-interrupt-only-fires-once
   - https://www.circuitstate.com/tutorials/making-two-raspberry-pi-pico-boards-communicate-through-spi-using-c-cpp-sdk/#SPI_Receive_Interrupt
   - https://www.digikey.ca/en/maker/projects/raspberry-pi-pico-rp2040-spi-example-with-micropython-and-cc/9706ea0cf3784ee98e35ff49188ee045
   - https://raspberrypi.stackexchange.com/questions/132758/what-is-the-pico-max-spi-frequency
   - https://github.com/espressif/esp-idf/issues/4810
   - https://forums.raspberrypi.com/viewtopic.php?t=30052

   - https://forums.raspberrypi.com/viewtopic.php?t=324252
   - https://github.com/raspberrypi/pico-examples/tree/master/spi/spi_dma
   - https://forums.raspberrypi.com/viewtopic.php?t=338108
   - https://people.ece.cornell.edu/land/courses/ece4760/RP2040/C_SDK_DMA_machine/DMA_machine_rp2040.html
   - https://blog.adafruit.com/2023/01/24/using-direct-memory-access-dma-on-the-rp2040-chip-raspberrypi-rp2040-dma
   - https://stackoverflow.com/questions/66388451/how-to-use-raspberry-pi-pico-with-dac-with-spi-to-generate-sine-wave-of-1-khz-wi

# Debouncing
- https://www.eejournal.com/article/ultimate-guide-to-switch-debounce-part-1


## BER
- https://www.ranecommercial.com/legacy/note161.html
- https://www.oss.com/asn1/resources/asn1-made-simple/asn1-quick-reference/basic-encoding-rules.html

```
48 37
   2 1 0 
   4 6 112 117 98 108 105 99
   160 24 
       2  1 1
       2  1 0
       2  1 0
       48 13 
          48 11 
             6 7 43 6 167 254 32 1 1
             5 0

::sequence        N:39  ix:2   length:37
  ::integer       N:37  ix:2   length:1  value:0
  ::octets        N:37  ix:5   length:6  value:public
  ::PDU           N:37  ix:13  length:24
    ::integer     N:24  ix:2   length:1  value:1
    ::integer     N:24  ix:5   length:1  value:0
    ::integer     N:24  ix:8   length:1  value:0
    ::sequence    N:24  ix:11  length:13
      ::sequence  N:13  ix:2   length:11
        ::OID     N:11  ix:2   length:7
        ::null    N:11  ix:11  length:0

OID
43  6 167 254 32  1  1
43 06 A7  FE  20 01 01

.1.3.....


✓ ::integer     N:37  ix:2   length:1  value:0
✓ ::octets      N:37  ix:5   length:6  octets:public
✓ ::integer     N:24  ix:2   length:1  value:1
✓ ::integer     N:24  ix:5   length:1  value:0
✓ ::integer     N:24  ix:8   length:1  value:0
✓ ::OID         N:11  ix:2   length:7  OID:0.1.3.6.655136.1.1
✓ ::null        N:11  ix:11  length:0
✓ ::sequence    N:13  ix:2   length:11  fields:2
✓ ::sequence    N:24  ix:11  length:13  fields:1
✓ ::PDU         N:37  ix:13  length:24  fields:4
✓ ::sequence    N:39  ix:2   length:37  fields:3

✓ >>> free 30
✓ >>> free 02
✓ >>> free 04
✓ >>> free a0
✓ >>> free 02
✓ >>> free 02
✓ >>> free 02
✓ >>> free 30
✓ >>> free 30
✓ >>> free 06
✓ >>> free 05

```
