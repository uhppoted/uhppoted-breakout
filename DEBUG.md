# DEBUG

Debugging weird intermittent reset:
- alarms/timers get disabled
- watchdog timer still working
- UART interrupts still working
- occasionally weird SSMP response even though message processing is commented out
```
... debug     264 16 16 02 30 0e 10 06 0a 2b 10 ....
... received 16 16 02 30 0e 10 06 0a
... received 2b 10 06 10 01 04 10 01
... received 84 80 00 10 02 10 01 10
... received 05 00 03 d3 89
... debug     265 16 16 02 30 0e 10 06 0a 2b 10 ....
... received 16 16 02 30 0e 10 06 0a
```

## Tests

| port | mode | SSMP | TTY     | Ok |
|------|------|------|---------|----|
| USB  | NONE | none | -       | ✓  |
| USB  | NONE | ENQ  | -       | ✓  |
| USB  | NONE | GET  | -       | ✗  |

## Possible causes

- ~~too many get requests~~
- ~~full queue~~
- ~~bisync decoder~~
- ~~UART interrupt handler~~
- ~~IOX or RTC~~
- ~~USB~~
- ~~loop in _print~~
- ~~CLI TERMINAL_QUERY_STATUS printf~~

- ?? only occurs with longer SSMP requests
- something in sequence ? 
- U3_read ?
- check all mutexes are try-lock (priority inversion) 

## TODO
- [ ] Check all FIXMEs
- [ ] Check that system doesn't freeze when print queue is full 
- [ ] Maybe only enable SSMP interrupt after a delay? 
- [ ] bisync max message size
- (?) bisync timeout
- [ ] Reinstate watchdog reset

