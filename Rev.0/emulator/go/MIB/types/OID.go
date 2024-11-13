package types

import (
	"fmt"
	"strings"
)

type OID []uint32

func (oid OID) String() string {
	var b strings.Builder

	if len(oid) > 0 && oid[0] == 0 {
		for _, v := range oid[1:] {
			fmt.Fprintf(&b, ".%v", v)
		}
	} else {
		for _, v := range oid {
			fmt.Fprintf(&b, ".%v", v)
		}
	}

	return b.String()
}
