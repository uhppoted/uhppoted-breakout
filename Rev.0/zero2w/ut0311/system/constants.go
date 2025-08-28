package system

type DoorMode uint8

const (
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
