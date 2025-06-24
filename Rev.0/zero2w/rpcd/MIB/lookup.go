package MIB

import (
	"errors"
)

var ErrUnknownOID = errors.New("unknown OID")

func Oid2Tag(oid string) (string, error) {
	if tag, ok := r[oid]; ok {
		return tag, nil
	}

	return "", ErrUnknownOID
}
