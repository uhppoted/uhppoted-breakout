package entities

import (
	"errors"
)

var ErrUnknownOID = errors.New("unknown OID")
var ErrRecordNotFound = errors.New("record not found")

const ErrMemory uint16 = 0x0001
const ErrQueue uint16 = 0x0002
const ErrI2C uint16 = 0x0004
const ErrRX8900 uint16 = 0x0008
const ErrU2 uint16 = 0x0010
const ErrU3 uint16 = 0x0020
const ErrU4 uint16 = 0x0040
const ErrWatchdog uint16 = 0x0080
const ErrDebug uint16 = 0x4000
const ErrUnknown uint16 = 0x8000
