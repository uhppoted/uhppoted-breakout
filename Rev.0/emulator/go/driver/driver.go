package driver

import (
	"emulator/MIB"
)

type Driver interface {
	Get(oid MIB.OID) (any, error)
	Set(oid MIB.OID, value any) (any, error)
}
