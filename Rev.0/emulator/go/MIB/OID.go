package MIB

import (
	"fmt"
	"slices"
	"strings"
)

type OID []uint32

func (o OID) String() string {
	var b strings.Builder

	if len(o) > 0 && o[0] == 0 {
		for _, v := range o[1:] {
			fmt.Fprintf(&b, ".%v", v)
		}
	} else {
		for _, v := range o {
			fmt.Fprintf(&b, ".%v", v)
		}
	}

	return b.String()
}

func (o OID) Equal(oid OID) bool {
	return slices.Equal(o, oid)
}
