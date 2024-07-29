# Simple Serial Management Protocol (SSMP)

## ENQ

Establishes connection, switching serial port from CLI to SSMP if necessary.

```
REQUEST:   SYN SYN ENQ
RESPONSE:  SYN SYN ACK
```

## GetRequest

Requests a PDU by MIB OID.

```
REQUEST:   SYN SYN SOH ... STX GET <OID><ETX>
RESPONSE:  SYN SYN ACK ... STX <value> <ETX>
```

