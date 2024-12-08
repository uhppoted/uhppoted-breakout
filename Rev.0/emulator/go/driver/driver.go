package driver

import (
	"emulator/MIB/types"
)

type Driver interface {
	Get(oid types.OID) (any, error)
}
