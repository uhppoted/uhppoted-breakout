# minimum-viable-bug

- [call to calloc() deadlocks with clang/picolibc](https://github.com/raspberrypi/pico-sdk/issues/1947)
- [Realloc() is not wrapped for multi-core safety](https://github.com/raspberrypi/pico-sdk/issues/863)
- [prevent self-deadlock with PICOLIBC due to calloc/realloc internally calling malloc/free](https://github.com/raspberrypi/pico-sdk/pull/2069)

## Traces
1. Base
```
     ----
>>>> TRACE 2025-01-01 09:40
     ID:  10
     in:  26372787
     out: 26372787
     stacktrace: 12
                 0  1   sys::dispatch
                 1  14  SSMP::receive
                 2  31  SSMP::BER
                 3  15  ASN.1::decode
                 4  21  ASN.1::unpack.seq
                 5  15  ASN.1::decode
                 6  17  ASN.1::unpack.int
                 7  24  ASN.1::int
                 8  25  ASN.1::int_calloc
                 9  5   U4::tick
                 10 6   U4::healthcheck
                 11 10  U4::calloc
```

2. Without U2
```
>>>> TRACE 2025-01-01 12:48
     ID:  10
     in:  704943
     out: 704943
     stacktrace: 9
                 0  1   sys::dispatch
                 1  14  SSMP::receive
                 2  35  SSMP::vector.free
                 3  37  vector::free
                 4  37  vector::free
                 5  37  vector::free
                 6  5   U4::tick
                 7  6   U4::healthcheck
                 8  10  U4::calloc
```

3. ??


## Notes
- https://forums.raspberrypi.com/viewtopic.php?t=361131
- https://github.com/raspberrypi/pico-sdk/issues/426
- https://forums.raspberrypi.com/viewtopic.php?t=336993
- https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib
