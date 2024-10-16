# NOTES

## Rev.1

1.  Schottkey diodes and resistor for RX8900SA VBAT
    - https://www.diodes.com/assets/Datasheets/SDM03U40.pdf
2.  Decoupling capacitor on RX8900SA VBAT
3.  (?) Tie FOE pin low through zero ohm resistor (with pullup to VDD)
4.  Pullups on INT0 and INT1
5.  Relays and reader LEDs should be open collector and tied to +5V (fail open and red LED_s
6.  (?) RESET should be tied low to hold U2 et al in reset until U5 is ready
7.  R1 and R2 - maybe increase resistance (2.5K? 10K?)
8.  1206 or 0804 or 0603 resistors and capacitors ?
9.  Connect NC pins on J10 to J2
10. Connect NC    pins on J12 to J12
11. Tie RX8900SA NC pins to GND
12. Fix PiZero2W mounting holes
    - https://www.tomshardware.com/news/waveshares-rp2040-pi-zero-morphs-raspberry-pi-pico-into-zero
13. PiZero2W header (surface mount)
14. Debug/state LEDs
15. BOM: 1.6mm x dia. 6.5mm washer for standoffs
16. BOM: M3x5 screws
17. BOM: Machine pin header and socket
18. Use smaller (2.0mm?) board-to-board connectors (J2,J3,J4)
19. Allow for external 3.3V
20. Replace Diodes Inc PI4IOE5V6416 with NXP equivalent
21. J10/J12 soldering jig
22. Test baseboard
23. Fix logo
24. Use machine pin socket on dev. board.
25. Relay startup click
26. SYS LED not driven
27. Reset/reboot button
28. Slot for USB ribbon
29. PiZero2W mounting holes are M2.5
30. Route GND and +5V to PiZero2W header (jumper on +5V)
31. Route SPI and UART to PiZero2W header (with solder bridges/zero ohm resistors)
32. DO and DI are actual D0 and D1, etc
(?) Maybe put GND on pin 1 of UART/SPI header
(?) https://www.olimex.com/Products/Components/IC/SY6280
    - https://hackaday.com/2024/07/03/usb-and-the-myth-of-500-milliamps/

(?) Mounting holes for external 12V power supply
(?) Slightly larger pads for soldering
(?) https://www.chipquik.com/store/product_info.php?products_id=1900050
(?) Jumper VBAT to VDD for no battery option
(?) https://www.flux.ai/p

### RX8900SA
1. RX8025

- https://electronics.stackexchange.com/questions/348095/coin-cell-battery-recharging-circuit-for-rtc-backup-power
- https://electronics.stackexchange.com/questions/410853/minimalistic-rtc-backup-battery-charging-or-a-super-capacitor
- https://electronics.stackexchange.com/questions/60690/will-checking-the-backup-voltage-on-an-rtc-drain-its-backup-battery
- https://electronics.stackexchange.com/questions/564726/how-can-i-measure-cr2032-rtc-battery-voltage-in-a-microprocessor-system
- https://www5.epsondevice.com/en/information/support/pdf/RTC-EVA2-Easy-instruction-ENG-Ver00.pdf
- https://www.mouser.ca/c/?marcom=126457092


## JLPCB
1. Solder stencil does not require BAT1 and U1 pads (large enough to hand solder).
2. Set custom stencil size when ordering.

## Misc.
1. Don't use flux paste on through holes (hard to clean, especially under jumpers and headers)


## Wiegand DI1/DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... f7 11110111  0 DO1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1
DEBUG  reader     ... fb 11111011  1 DI1

10058399
0C9C83F
0 0110 0100 1110 0100 0001 1111 1

0 0110 0100 1110 0100 0001 1111 1

