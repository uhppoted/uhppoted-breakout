package ssmp

import (
	"strconv"
	"sync/atomic"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/go/crypt/otp"
)

var id = atomic.Uint64{}

func nextID() (uint32, error) {
	next := id.Add(1)

	if code, err := otp.GenerateHOTP("A3WYPV4VWQLL6VFZ", next); err != nil {
		return 0, err
	} else if v, err := strconv.ParseUint(code, 10, 32); err != nil {
		return 0, err
	} else {
		return uint32(v), nil
	}
}