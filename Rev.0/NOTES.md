# NOTES

## Rev.1
1. Schottkey diodes and resistor for RX8900SA VBAT
   - https://www.diodes.com/assets/Datasheets/SDM03U40.pdf
2. Decoupling capacitor on RX8900SA VBAT
3. (?) Tie FOE pin low through zero ohm resistor (with pullup to VDD)
4. Pullups on INT0 and INT1
5. RESET should (maybe) be tied low to hold U2 et al in reset until U5 is ready
6 R1 and R2 - maybe increase resistance (2.5K? 10K?)
7. Connect NC pins on J10 to J2
8. Connect NC    pins on J12 to J12
9. Tie RX8900SA NC pins to GND
10. Fix PiZero2W mounting holes
11. PiZero2W header (?)
12. debug/state LEDs
13. BOM: 1.6mm x dia. 6.5mm washer for standoffs
14. BOM: M3x5 screws
15. BOM: Machine pin header and socket
16. Use smaller (2.0mm?) board-to-board connectors (J2,J3,J4)
17. Allow for external 3.3V
18. Replace Diodes Inc PI4IOE5V6416 with NXP equivalent
19. J10/J12 soldering jig
20. Test baseboard
21. 0805 resistors and capacitors
22. Fix logo
23. Use machine pin socket on dev. board.

(?) Slightly larger pads for soldering
(?)  https://www.chipquik.com/store/product_info.php?products_id=1900050
(?) Pulldowns on all inputs
(?) RC filters on inputs
(?) Expose FOUT pin via test point/pad and resistor
(?) Expose INT  pin via test point/pad and resistor
(?) Jumper VBAT to VDD for no battery option

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

