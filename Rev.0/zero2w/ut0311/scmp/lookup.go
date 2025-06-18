package scmp

import (
	"ut0311/entities"
)

var m = map[string]string{
	"controller.door.4.button": "0.1.3.6.1.4.1.65536.3.4.6",
}

var r = map[string]string{
	"0.1.3.6.1.4.1.65536.3.4.6": "controller.door.4.button",
}

func Oid2Tag(oid string) (string, error) {
	if tag, ok := r[oid]; ok {
		return tag, nil
	}

	return "", entities.ErrUnknownOID
}
