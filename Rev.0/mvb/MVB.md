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

- https://community.st.com/t5/stm32cubeide-mcus/newlib-malloc-locking-mechanism-to-be-thread-safe/td-p/240082
- https://www.nadler.com/embedded/newlibAndFreeRTOS.html
- https://stackoverflow.com/questions/39088598/malloc-in-newlib-does-it-waste-memory-after-one-big-failure-allocation
- https://esp32.com/viewtopic.php?t=8419
- https://forums.freertos.org/t/malloc-free-thread-interrupt-safe/3452
- https://stackoverflow.com/questions/43222519/malloc-alternative-for-interrupt-safety
- https://stackoverflow.com/questions/855763/is-malloc-thread-safe
- https://github.com/eblot/newlib/blob/master/newlib/libc/sys/linux/malloc.c
- https://github.com/ARM-software/CMSIS_5/issues/177
- https://www.reddit.com/r/embedded/comments/9uvogb/freertos_malloc_and_c_standard_library/


> No, it is not thread-safe. There may actually be a malloc_lock() and malloc_unlock() function available in your
> C library. I know that these exist for the Newlib library. I had to use this to implement a mutex for my
> processor, which is multi-threaded in hardware.
