package entities

import (
	"errors"
)

var ErrUnknownOID = errors.New("unknown OID")
var ErrRecordNotFound = errors.New("record not found")
