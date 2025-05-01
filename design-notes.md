# DESIGN NOTES

## UT0311L04

### Power Consumption

| Configuration    | Measured        | Power  |
|------------------|-----------------|--------|
| Base + CPU       | 155mA  @ 12.21V | 1.9W   |
| Base             | 16.5mA @ 12.23V | 0.2W   |
| Breakout Rev 0.0 | 25.4mA @ 5.10V  | 0.13W  |

### Startup

| RELAY | N/O     | N/C    | LED |
|-------|---------|--------|-----|
| 1     | OPEN    | CLOSED | +5V |
| 2     | OPEN    | CLOSED | +5V |
| 3     | OPEN    | CLOSED | +5V |
| 4     | OPEN    | CLOSED | +5V |


### Relays
1. +5V/(1k || 10.470k) = 4.5mA
2. ULN2003A = 5V * 10k(10k + 470) = 4.77V

### Reader LEDs
1. +5V/470 = 10.6mA
2. Assumed to be high impendance signal (not driving LED directly)

### SYS, ERR and IN LEDs
1. (+5V - D5 - D6)/1k = (+5V - 0.6 - 0.6)/1k = 3.8mA

### References
1. https://www.cnx-software.com/2021/12/09/raspberry-pi-zero-2-w-power-consumption
2. https://raspi.tv/2017/how-much-power-does-pi-zero-w-use


## PCAL6408APW

### Input Schmmitt trigger
- Vᴅᴅ   = 5V
- Vʟᴏᴡ  = 0.3Vᴅᴅ (1.5V)
- Vʜɪɢʜ = 0.7Vᴅᴅ (3.5V)
- Vʜ    = 3.5V - 1.5V = 2V

## SSMP
1. Does not handle bignums.

