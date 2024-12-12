package driver

import (
	"emulator/MIB/types"
)

type Driver interface {
	Get(oid types.OID) (any, error)
	Set(oid types.OID, value any) (any, error)
}
