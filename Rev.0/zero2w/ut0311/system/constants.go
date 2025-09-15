package system

type DoorMode uint8

const (
	ModeUnknown    DoorMode = 0
	NormallyOpen   DoorMode = 1
	NormallyClosed DoorMode = 2
	Controlled     DoorMode = 3
)

var modes = map[uint8]DoorMode{
	1: NormallyOpen,
	2: NormallyClosed,
	3: Controlled,
}

func (m DoorMode) String() string {
	switch m {
	case NormallyOpen:
		return "normally-open"

	case NormallyClosed:
		return "normally-closed"

	case Controlled:
		return "controlled"

	default:
		return "unknown"
	}
}

type Interlock uint8

const (
	NoInterlock      Interlock = 0x00 // disabled
	Interlock12      Interlock = 0x01 // doors (1,2)
	Interlock34      Interlock = 0x02 // doors (3,4)
	Interlock12_34   Interlock = 0x03 // doors (1,2) and (3,4)
	Interlock123     Interlock = 0x04 // doors (1,2,3)
	Interlock1234    Interlock = 0x08 // doors (1,2,3,4)
	InterlockUnknown Interlock = 0xff // unknown
)

var interlocks = map[uint8]Interlock{
	0x00: NoInterlock,
	0x01: Interlock12,
	0x02: Interlock34,
	0x03: Interlock12_34,
	0x04: Interlock123,
	0x08: Interlock1234,
}
