# minimum-viable-bug

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
     ----
```

2. 