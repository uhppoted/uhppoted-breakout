# DESIGN NOTES

## UT0311L04

## Power Consumption

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


## PCAL6408APW

### Input Schmmitt trigger
- Vᴅᴅ   = 5V
- Vʟᴏᴡ  = 0.3Vᴅᴅ (1.5V)
- Vʜɪɢʜ = 0.7Vᴅᴅ (3.5V)
- Vʜ    = 3.5V - 1.5V = 2V

## SSMP
1. Does not handle bignums.

