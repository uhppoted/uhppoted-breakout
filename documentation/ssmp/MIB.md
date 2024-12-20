# MIB

## Internal

| OID                     | Description                 | Notes      |
|-------------------------|-----------------------------|------------|
|0.1.3.6.1.4.1.65536.0    | internal                    |            |
|0.1.3.6.1.4.1.65536.0.0  | compliance statement        |            |
|0.1.3.6.1.4.1.65536.0.1  | system object group         |            |
|0.1.3.6.1.4.1.65536.0.2  | controller object group     |            |
|0.1.3.6.1.4.1.65536.0.3  | doors object group          |            |
|0.1.3.6.1.4.1.65536.0.4  | inputs object group         |            |
|0.1.3.6.1.4.1.65536.0.5  | events object group         |            |


## System

| OID                      | Description                 | Notes      |
|--------------------------|-----------------------------|------------|
|0.1.3.6.1.4.1.65536.1     | system                      | 
|0.1.3.6.1.4.1.65536.1.1   | RP2040 serial number        | _private_  |


## Controller

| OID                       | Description                | Notes      |
|---------------------------|----------------------------|------------|
| 0.1.3.6.1.4.1.65536.2     | controller                 | _public_   |
| 0.1.3.6.1.4.1.65536.2.1   | controller ID              | _public_   |
| 0.1.3.6.1.4.1.65536.2.2   | controller version         | _public_   |
| 0.1.3.6.1.4.1.65536.2.3   | controller released        | _public_   |
| 0.1.3.6.1.4.1.65536.2.4   | controller address         | _public_   |
| 0.1.3.6.1.4.1.65536.2.5   | controller netmask         | _public_   |
| 0.1.3.6.1.4.1.65536.2.6   | controller gateway         | _public_   |
| 0.1.3.6.1.4.1.65536.2.7   | controller MAC             | _public_   |
| 0.1.3.6.1.4.1.65536.2.8   | controller date/time       | _public_   |
| 0.1.3.6.1.4.1.65536.2.9   | controller error           | _public_   |
| 0.1.3.6.1.4.1.65536.2.10  | controller special info    | _public_   |
| 0.1.3.6.1.4.1.65536.2.11  | controller sequence number | _public_   |
| 0.1.3.6.1.4.1.65536.2.12  | controller event listener  | _public_   |
| 0.1.3.6.1.4.1.65536.2.13  | controller upload interval | _public_   |


## Doors

| OID                       | Description                | Notes      |
|---------------------------|----------------------------|------------|
| 0.1.3.6.1.4.1.65536.3     | doors                      | _public_   |
| 0.1.3.6.1.4.1.65536.3.1.1 | door 1 mode                | _public_   |
| 0.1.3.6.1.4.1.65536.3.1.2 | door 1 delay               | _public_   |
| 0.1.3.6.1.4.1.65536.3.1.3 | door 1 unlocked            | _public_   |
| 0.1.3.6.1.4.1.65536.3.1.4 | door 1 open                | _public_   |
| 0.1.3.6.1.4.1.65536.3.1.5 | door 1 button pressed      | _public_   |
| 0.1.3.6.1.4.1.65536.3.2.1 | door 2 mode                | _public_   |
| 0.1.3.6.1.4.1.65536.3.2.2 | door 2 delay               | _public_   |
| 0.1.3.6.1.4.1.65536.3.2.3 | door 2 unlocked            | _public_   |
| 0.1.3.6.1.4.1.65536.3.2.4 | door 2 open                | _public_   |
| 0.1.3.6.1.4.1.65536.3.2.5 | door 2 button pressed      | _public_   |
| 0.1.3.6.1.4.1.65536.3.3.1 | door 3 mode                | _public_   |
| 0.1.3.6.1.4.1.65536.3.3.2 | door 3 delay               | _public_   |
| 0.1.3.6.1.4.1.65536.3.3.3 | door 3 unlocked            | _public_   |
| 0.1.3.6.1.4.1.65536.3.3.4 | door 3 open                | _public_   |
| 0.1.3.6.1.4.1.65536.3.3.5 | door 3 button pressed      | _public_   |
| 0.1.3.6.1.4.1.65536.3.4.1 | door 4 mode                | _public_   |
| 0.1.3.6.1.4.1.65536.3.4.2 | door 4 delay               | _public_   |
| 0.1.3.6.1.4.1.65536.3.4.3 | door 4 unlocked            | _public_   |
| 0.1.3.6.1.4.1.65536.3.4.4 | door 4 open                | _public_   |
| 0.1.3.6.1.4.1.65536.3.4.5 | door 4 button pressed      | _public_   |


## Inputs

| OID                       | Description                | Notes      |
|---------------------------|----------------------------|------------|
| 0.1.3.6.1.4.1.65536.4     | inputs                     | _public_   |
| 0.1.3.6.1.4.1.65536.4.1   | tamper detect              | _public_   |
| 0.1.3.6.1.4.1.65536.4.2   | fire alarm                 | _public_   |


## Cards

| OID                       | Description                | Notes      |
|---------------------------|----------------------------|------------|
| 0.1.3.6.1.4.1.65536.5     | cards                      | _public_   |



## Events

| OID                       | Description                | Notes      |
|---------------------------|----------------------------|------------|
| 0.1.3.6.1.4.1.65536.6     | events                     | _public_   |
| 0.1.3.6.1.4.1.65536.6.0.1 | first event id             | _public_   |
| 0.1.3.6.1.4.1.65536.6.0.2 | last event id              | _public_   |
|                           |                            | _public_   |
| 0.1.3.6.1.4.1.65536.6.x.1 | event X event              | _public_   |
| 0.1.3.6.1.4.1.65536.6.x.2 | event X granted            | _public_   |
| 0.1.3.6.1.4.1.65536.6.x.3 | event X door               | _public_   |
| 0.1.3.6.1.4.1.65536.6.x.4 | event X direction          | _public_   |
| 0.1.3.6.1.4.1.65536.6.x.5 | event X card               | _public_   |
| 0.1.3.6.1.4.1.65536.6.x.6 | event X timestamp          | _public_   |
| 0.1.3.6.1.4.1.65536.6.x.7 | event X reason             | _public_   |
