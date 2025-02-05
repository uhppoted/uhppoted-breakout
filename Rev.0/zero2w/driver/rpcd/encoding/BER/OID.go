package BER

import (
	"fmt"
	"slices"
	"strings"
)

type OID []uint32

func ParseOID(oid string) (OID, error) {
	return nil, fmt.Errorf("ALSO NOT IMPLEMENTED")
}

func Equal(oid1 OID, oid2 OID) bool {
	return slices.Equal(oid1, oid2)
}

func (o OID) String() string {
	s := make([]string, len(o))

	for i, v := range o {
		s[i] = fmt.Sprintf("%v", v)
	}

	return strings.Join(s, ".")
}
