package stub

import (
	"emulator/MIB/types"
	"emulator/log"
)

type Stub struct {
}

func (s Stub) Get(oid types.OID) (uint32, error) {
	debugf("get %v\n", oid)

	return 405419896, nil
}

func debugf(format string, args ...any) {
	log.Debugf("STUB", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("STUB", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("STUB", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("STUB", format, args...)
}
