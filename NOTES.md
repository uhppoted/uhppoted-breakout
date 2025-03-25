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
21. Gateway discovery
    - https://github.com/jackpal/gateway
    - https://gist.github.com/jftuga/18409ced62f9a59ecbc047800ab8e810
    - https://forum.golangbridge.org/t/get-set-network-interface-in-pure-go-code/9075/3
22. [Memory usage](https://forums.raspberrypi.com/viewtopic.php?t=347638)
23. https://electronics.stackexchange.com/questions/742062/robust-communication-between-%c2%b5p-board-and-i-o-expander-over-3-5-meters

## WaveShare RP2040-Tiny
1. https://tinygo.org/docs/reference/microcontrollers/machine/waveshare-rp2040-zero
```
On some boards, the XOSC can take longer than usual to stabilize. On such boards, this is needed to avoid a hard fault on boot/reset. Refer to PICO_XOSC_STARTUP_DELAY_MULTIPLIER in the Pico SDK for additional details.
``` 

## malloc/free
- https://forums.raspberrypi.com/viewtopic.php?t=361131
- https://github.com/raspberrypi/pico-sdk/issues/426
- https://forums.raspberrypi.com/viewtopic.php?t=336993
- https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib

- https://community.st.com/t5/stm32cubeide-mcus/newlib-malloc-locking-mechanism-to-be-thread-safe/td-p/240082
- https://www.nadler.com/embedded/newlibAndFreeRTOS.html
- https://stackoverflow.com/questions/39088598/malloc-in-newlib-does-it-waste-memory-after-one-big-failure-allocation
- https://esp32.com/viewtopic.php?t=8419
- https://forums.freertos.org/t/malloc-free-thread-interrupt-safe/3452
- https://stackoverflow.com/questions/43222519/malloc-alternative-for-interrupt-safety
- https://stackoverflow.com/questions/855763/is-malloc-thread-safe
- https://github.com/eblot/newlib/blob/master/newlib/libc/sys/linux/malloc.c
- https://github.com/ARM-software/CMSIS_5/issues/177
- https://www.reddit.com/r/embedded/comments/9uvogb/freertos_malloc_and_c_standard_library/


> No, it is not thread-safe. There may actually be a malloc_lock() and malloc_unlock() function available in your
> C library. I know that these exist for the Newlib library. I had to use this to implement a mutex for my
> processor, which is multi-threaded in hardware.

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

## Debouncing
- https://www.eejournal.com/article/ultimate-guide-to-switch-debounce-part-1

## USB
   - https://forums.raspberrypi.com/viewtopic.php?t=302960
   - https://stackoverflow.com/questions/29354228/is-stdout-required-to-be-a-text-stream

## CRCs
- https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum

## ASN.1
- https://learn.microsoft.com/en-gb/windows/win32/seccertenroll/about-integer?redirectedfrom=MSDN
- https://www.ranecommercial.com/legacy/note161.html
- https://www.oss.com/asn1/resources/asn1-made-simple/asn1-quick-reference/basic-encoding-rules.html

## SSMP
- https://stackoverflow.com/questions/29354228/is-stdout-required-to-be-a-text-stream
- https://forums.raspberrypi.com/viewtopic.php?t=302960
- https://www.baeldung.com/cs/bisync
- [PPP](https://datatracker.ietf.org/doc/html/rfc1661)
- https://learn.microsoft.com/en-us/windows/win32/snmp/snmp-error-codes

### Serial I/O
- https://stackoverflow.com/questions/51609813/serial-communication-in-c-for-raspberry-pi
- https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
- https://tldp.org/HOWTO/Serial-Programming-HOWTO/x56.html
- https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum
- https://stackoverflow.com/questions/76367736/uart-tx-produce-endless-interrupts-how-to-acknowlage-the-interrupt

## Crypto
- https://www.jstage.jst.go.jp/article/transinf/E99.D/8/E99.D_2015INP0013/_article
- https://github.com/ubirch/ubirch-mbed-nacl-cm0
- https://www.semanticscholar.org/paper/True-random-number-generation-on-an-Atmel-AVR-Hlav%C3%A1c-L%C3%B3rencz/3948b47a9fb7a6b3632a1b631035bec3972932bc

## TLS
- https://eli.thegreenplace.net/2021/go-socket-servers-with-tls
- https://www.electricmonk.nl/log/2018/06/02/ssl-tls-client-certificate-verification-with-python-v3-4-sslcontext/

## Flash ID
1. https://forums.raspberrypi.com/viewtopic.php?t=331351
2. https://forums.raspberrypi.com/viewtopic.php?t=316853
3. https://forums.raspberrypi.com/viewtopic.php?t=316853
