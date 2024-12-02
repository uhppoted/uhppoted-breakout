# DEBUG

Debugging weird intermittent reset:
- alarms/timers get disabled
- watchdog timer still working
- UART interrupts still working
- seemed to freeze very quickly with put_led in SSMP::received
  - unexpected interrupt handling chain somewhere??
  - PIO ??

## Tests

| port | mode | SSMP       | TTY     | Ok | Notes                               |
|------|------|------------|---------|----|-------------------------------------|
| USB  | NONE | ASN.1+none | -       | ✓  |                                     |
| USB  | NONE | ASN.1+ENQ  | -       | ✓  |                                     |
| USB  | NONE | ASN.1+SEQ  | -       | ✗  | Hangs after about an hour           |
| USB  | NONE | bisync+SEQ | -       | ✓  |                                     |
| USB  | NONE | bisync+GET | -       | ✓  |                                     |
| USB  | NONE | ASN.1+SEQ  | -       |    | _vector_ without any decoded fields |

## Possible causes

- ~~too many get requests~~
- ~~full queue~~
- ~~bisync decoder~~
- ~~UART interrupt handler~~
- ~~IOX or RTC~~
- ~~USB~~
- ~~loop in _print~~
- ~~CLI TERMINAL_QUERY_STATUS printf~~
- ~~only occurs with longer SSMP requests~~
- ~~occasional SSMP echo although message processing is commented out~~ (in reset/BOOTSEL the RP2040 seems to echo input on GPIO 4/5)

- something in sequence ? 
  - check logic around unpack_sequence and *ix
- check all mutexes are try-lock (priority inversion) 
- U3_read ?

## TODO
- [ ] Check all FIXMEs
- [ ] Check that system doesn't freeze when print queue is full 
- [ ] Maybe only enable SSMP interrupt after a delay? 
- [ ] bisync max message size
- (?) bisync timeout
- [ ] Reinstate watchdog reset

