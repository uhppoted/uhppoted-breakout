# NOTES

## Rev.1
1. Connect NC pins on J10 to J2
2. Connect NC    pins on J12 to J12
3. Need 1.6mm x dia. 6.5mm washer for standoffs
4. Use smaller (2.0mm?) board-to-board connectors (J2,J3,J4)
5. Allow for external 3.3V
6. Replace Diodes Inc PI4IOE5V6416 with NXP equivalent
7. (?) RESET should (maybe) be tied low to hold U2 et al in reset until U5 is ready
8. (?) Pullup/down resistors for reader LEDs
9. J10/J12 soldering jig
   - 2x10P header pins
10. https://www.chipquik.com/store/product_info.php?products_id=1900050
11. R1 and R2 - maybe increase resistance (2.5K? 10K?)
12. Slightly larger pads for soldering
13. 0805 resistors and capacitors
14. Fix logo
16. Use machine pin socket on dev. board.

### RX8900SA
1. Decoupling capacitor on VBAT
2. Tie NC pin to GND
3. Diode and resistor to VBAT
   - https://www.diodes.com/assets/Datasheets/SDM03U40.pdf
4. RX8025 ??
5. (?) Jumper VBAT to VDD for no battery option
6. (?) Tie FOE pin low through zero ohm resistor (with pullup to VDD)
7. (?) Expose FOUT pin via test point/pad and resistor
8. (?) Expose INT  pin via test point/pad and resistor

- https://electronics.stackexchange.com/questions/348095/coin-cell-battery-recharging-circuit-for-rtc-backup-power
- https://electronics.stackexchange.com/questions/410853/minimalistic-rtc-backup-battery-charging-or-a-super-capacitor
- https://electronics.stackexchange.com/questions/60690/will-checking-the-backup-voltage-on-an-rtc-drain-its-backup-battery
- https://electronics.stackexchange.com/questions/564726/how-can-i-measure-cr2032-rtc-battery-voltage-in-a-microprocessor-system
- https://www5.epsondevice.com/en/information/support/pdf/RTC-EVA2-Easy-instruction-ENG-Ver00.pdf


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

