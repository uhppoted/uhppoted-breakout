# DEBUG

Debugging weird intermittent reset:
- alarms/timers get disabled
- watchdog timer still working
- UART interrupts still working
- seemed to freeze very quickly with put_led in SSMP::received
  - unexpected interrupt handling chain somewhere??
  - PIO ??

- [Timer callback gets stuck when thread is running](https://github.com/orgs/micropython/discussions/10700)
- [Timer callback stops with uart thread](https://forum.micropython.org/viewtopic.php?f=21&t=12194&p=66161&hilit=pico+thread#p66161)
- [Problem with machine.Timer() and _thread on Raspberry Pi Pico (RP2040)](https://stackoverflow.com/questions/70048254/problem-with-machine-timer-and-thread-on-raspberry-pi-pico-rp2040)
- [Repeating timer stops](https://forums.raspberrypi.com/viewtopic.php?t=378705)
- [alarm pool can appear to stop working](https://github.com/raspberrypi/pico-sdk/issues/1953)
- [stdio_uart and stdio_uart_init_full don't play nice with flush](https://github.com/raspberrypi/pico-sdk/issues/1936)

## Tests

| port | mode | SSMP       | TTY     | Ok | Notes                               |
|------|------|------------|---------|----|-------------------------------------|
| USB  | NONE | ASN.1+none | -       | ✓  |                                     |
| USB  | NONE | ASN.1+ENQ  | -       | ✓  |                                     |
| USB  | NONE | ASN.1+SEQ  | -       | ✗  | Hangs after about an hour           |
| USB  | NONE | bisync+SEQ | -       | ✓  |                                     |
| USB  | NONE | bisync+GET | -       | ✓  |                                     |
| USB  | NONE | ASN.1+SEQ  | -       | ✓  | _vector_ without any decoded fields |
| USB  | NONE | ASN.1+SEQ  | -       | ✓  | _vector_ without NULL fields        |
| USB  | NONE | ASN.1+SEQ  | -       | ✗  | _vector_ without vector_new()       |

| RTC | U2 | U3 | U4 | LOG | CLI(USB) | CLI(UART0) | SSMP   | Ok | Notes                |
|-----|----|----|----|-----|----------|------------|--------|----|----------------------|
|  ✗  | ✗  | ✗  | ✗  |  ✗  |    ✗     |     ✗      | BER    | ✓  | 16hrs , no glitch    |
|  ✓  | ✗  | ✗  | ✗  |  ✓  |    ✗     |     ✗      | BER    | ✓  | 8hrs , no glitch     |
|  ✓  | ✓  | ✗  | ✗  |  ✓  |    ✗     |     ✗      | BER    | ✓  | 9hrs , no glitch     |
|  ✓  | ✓  | ✓  | ✗  |  ✓  |    ✗     |     ✗      | decode | ✓  | 6hrs , no glitch     |
|  ✓  | ✓  | ✓  | ✓  |  ✓  |    ✗     |     ✗      | decode | ✗  | **glitched**         |

### U4

| init | setup | relays | LEDs | outputs | monitor | Ok |
|------|-------|--------|------|---------|---------|----|
|  ✓   |   x   |   x    |  x   |   x     |    x    | ✓  |
|  ✓   |   ✓   |   x    |  x   |   x     |    x    | ✓  |
|  ✓   |   ✓   |   ✓    |  x   |   x     |    x    | ✓  |
|  ✓   |   ✓   |   ✓    |  x   |   ✓     |    x    | ✓  |
|  ✓   |   ✓   |   ✓    |  ✓   |   ✓     |    x    | ✓  |
|  ✓   |   ✓   |   ✓    |  ✓   |   ✓     |    ✓    | x  |

### U4::healthcheck

| push | op::NULL | set_error | calloc |       |       | Ok | Notes           |
|------|----------|-----------|--------|-------|-------|----|-----------------|
|  ✓   |   x      |     ✓     |   ✓    |       |       | x  |                 |
|  ✓   |   ✓      |     ✓     |   ✓    |       |       | x  |                 |
|  x   |   ✓      |     ✓     |   ✓    |       |       | x  |                 |
|  x   |   ✓      |     x     |   ✓    |       |       | x  |                 |
|  x   |   x      |     x     |   x    |       |       |    |                 |

```
     ----
>>>> TRACE
     ID:  5
     in:  3866904
     out: 3866904
     stacktrace: 3
                 0  1 TRACE_SYS_DISPATCH
                 1  7 TRACE_SSMP_RECEIVE
                 2  5 TRACE_U4_TICK
```
```
>>>> TRACE
     ID:  6
     in:  26473970
     out: 26473970
     stacktrace: 4
                 0  1   sys::dispatch
                 1  11  SSMP::receive
                 2  5   U4::tick
                 3  6   U4::healthcheck
     ----
```

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
- ~~occasional SSMP echo although message processing is commented out~~ (in reset/BOOTSEL -needs pullups on the TX/RX pins)
- ~~check logic around unpack_sequence and *ix~~

- corrupted message ??
- something in sequence ? 
- check all mutexes are try-lock (priority inversion) 
- U3_read ?
- interrupt interrupting interrupt ?
- [clocks_enable_resus](https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#group_hardware_clocks_1ga614700afaa5ee6767ef2cf662e63f84c)

- [ ] Print out timer registers
      - [x] 'now'
- [x] Disable RTC update
- [x] Increase request rate
- [x] Mark head/tail as volatile
- [ ] Mark head/tail as atomic (https://en.cppreference.com/w/c/language/atomic)
- [ ] ... and then maybe make it a queue 

## TODO
- [ ] Check all FIXMEs
- [ ] Check that system doesn't freeze when print queue is full 
- [ ] Maybe only enable SSMP interrupt after a delay? 
- [ ] bisync max message size
- (?) bisync timeout
- [ ] Reinstate watchdog reset

## GET

48 41 2 1 0 4 6 112 117 98 108 105 99 160 28 2 2 48 57 2 1 0 2 1 0 48 16 48 14 6 10 43 6 1 4 1 132 128 0 2 1 5 0

48 41                         SEQUENCE
   2 1 0                      INTEGER 0
   4 6 112 117 98 108 105 99  STRING  public
   160 28                     PDU     GET
       2 2 48 57              INTEGER
       2 1 0                  INTEGER error:0
       2 1 0                  INTEGER error index:0
       48 16                  SEQUENCE
          48 14               SEQUENCE
             6 10 43 6 1 4 1 132 128 0 2 1    OID ".1.3.6.1.4.1.65536.2.1"
             5 0                              NULL
