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

type AntiPassback uint8

const (
	NoAntiPassback      AntiPassback = 0x00 // disabled
	Readers12_34        AntiPassback = 0x01 // readers 1:2; 3:4 (independently)
	Readers13_24        AntiPassback = 0x02 // readers (1,3):(2,4)
	Readers1_23         AntiPassback = 0x03 // readers 1:(2,3)
	Readers1_234        AntiPassback = 0x04 // readers 1:(2,3,4)
	AntiPassbackUnknown AntiPassback = 0xff // unknown
)

func (a AntiPassback) String() string {
	switch a {
	case NoAntiPassback:
		return "disabled"

	case Readers12_34:
		return "(1:2);(3:4)"

	case Readers13_24:
		return "(1,3):(2,4)"

	case Readers1_23:
		return "1:(2,3)"

	case Readers1_234:
		return "1:(2,3,4)"

	default:
		return "unknown"
	}
}

var antipassbacks = map[uint8]AntiPassback{
	0x00: NoAntiPassback,
	0x01: Readers12_34,
	0x02: Readers13_24,
	0x03: Readers1_23,
	0x04: Readers1_234,
}
