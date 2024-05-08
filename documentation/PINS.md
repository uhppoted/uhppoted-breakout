# PINs

## Base board

| J11                           || J10                            |
|----|---------|----|-----------||----|----------|----|-----------|
| 20 | DO1     | 1  | +5VDC     || 20 | SCL      | 1  | SDA       |
| 19 | DI1     | 2  |  DI4      || 19 | PB4      | 2  | S1        |
| 18 | DO2     | 3  |  DO4      || 18 | PB3      | 3  | S2        |
| 17 | DI2     | 4  |  DI3      || 17 | PB2      | 4  | S3        |
| 16 | NC      | 5  |  DO3      || 16 | PB1      | 5  | S4        |
| 15 | DR1 LED | 6  |  RELAY1   || 15 | SYS LED  | 6  | NC        |
| 14 | DR2 LED | 7  |  RELAY2   || 14 | IN  LED  | 7  | NC        |
| 13 | DR3 LED | 8  |  RELAY3   || 13 | ERR LED  | 8  | NC        |
| 12 | DR4 LED | 9  |  RELAY4   || 12 | VREF     | 9  | GND       |
| 11 | GND     | 10 |  VREF     || 11 | NC       | 10 | NC        |
|----|---------|----|-----------||----|----------|----|-----------|

## CPU board

| J10                           || J12                           |
|----|----------|----|----------||----|-----------|----|---------|
| 1  | SDA      | 20 | SCL      || 1  | +5VDC     | 20 | DO1     |
| 2  | S1       | 19 | PB4      || 2  |  DI4      | 19 | DI1     |
| 3  | S2       | 18 | PB3      || 3  |  DO4      | 18 | DO2     |
| 4  | S3       | 17 | PB2      || 4  |  DI3      | 17 | DI2     |
| 5  | S4       | 16 | PB1      || 5  |  DO3      | 16 | NC      |
| 6  | NC       | 15 | SYS LED  || 6  |  RELAY1   | 15 | DR1 LED |
| 7  | NC       | 14 | IN  LED  || 7  |  RELAY2   | 14 | DR2 LED |
| 8  | NC       | 13 | ERR LED  || 8  |  RELAY3   | 13 | DR3 LED |
| 9  | GND      | 12 | VREF     || 9  |  RELAY4   | 12 | DR4 LED |
| 10 | NC       | 11 | NC       || 10 |  VREF     | 11 | GND     |
|----|----------|----|----------||----|-----------|----|---------|


## Miscellaneous

1.  3.3VDC supply seems to be on CPU board
    - AMS1117
2.  Connectors are 2x10P 2mm pitch
3.  FLJ9 is just GND and +5VDC
4.  Mounting holes are 3.5mm dia.
5.  CPU board
    - width: 63.25mm (63.5mm = 2.5")
    - height: 75mm (76.2mm = 3")
6.  J10.1 to J12.1 is 41.5mm (40.64mm = 16*2.54)
7.  Single mounting hole:
    - centre to J10.1: 21.75mm
    - centre to J12.1 20.0mmmm
    - because 2mm header column spacing
8.  Double mounting holes
    - ~65mm    from single mounting hole
    - ~64.25mm from single mounting hole
    - (64.1 + 64.8)/2 = 64.45
    - 68.0 - 3.5 = 64.5
    - 64.77 = 51*1.27
    - distance between = 25mm
    - ** offset 64.5mm
    - ** seperation 25mm
9.  Silkscreen has used pin header footprint for sockets J10 and J12 on CPU board
10. Silkscreen has used pin socket footprint for header J10 and J11 on base board


### Remeasured

1. J11 P1 to J10 P1: 40.9mm (but 2.3mm measured is 2mm pin-to-pin)
2. J11 P1 to mounting hole centre: 21.5 (~20.32 ??)
3. Mounting hole centre in line with pin 9 of J10/J11
4. Mounting hole center to J11 pin 1 is 50.0mm
5. Mounting hole center to J10 pin 1 is 59.0mm

## Wiegand Reader I/O

1. 470 + 10k ohms series resistance
2. ??  capacitor
3. LPF presumably
4. VREF is the nominal +5VDC supplied to the reader digital pins
5. VREF is common on the 'other side' of the resistors


## Relays

1. Nominally 12V relay
2. Active high:
   - locked: 0VDC
   - unlocked: 2.6VDC
3. RELAY 1 - R42 - NXP p40
4. RELAY 2 - R43 - NXP p50
5. RELAY 3 - R44 - NXP p44
6. RELAY 4 - R45 - NXP p45



## LEDs

1. On board DOOR LEDs are probably on relay signal
2. READER LEDs are 5V pulled to GND
3. SYS, IN and ERR LED are 3.3V pulled to GND


## SIO

1. SCL is 3.3V, pullup
2. SDA is 3.3V, pullup

## FLJ9

## Connectors

