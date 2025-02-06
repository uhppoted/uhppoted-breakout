package BER

import (
	"fmt"
	"slices"
	"strconv"
	"strings"
)

type OID []uint32

func ParseOID(s string) (OID, error) {
	var tokens []string
	var oid []uint32

	if strings.HasPrefix(s, ".") {
		tokens = strings.Split(s[1:], ".")
		oid = make([]uint32, 0, len(tokens)+1)
		oid = append(oid, 0)
	} else {
		tokens = strings.Split(s, ".")
		oid = make([]uint32, 0, len(tokens))
	}

	for _, t := range tokens {
		if v, err := strconv.ParseUint(t, 10, 32); err != nil {
			return nil, err
		} else {
			oid = append(oid, uint32(v))
		}
	}

	return OID(oid), nil
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
