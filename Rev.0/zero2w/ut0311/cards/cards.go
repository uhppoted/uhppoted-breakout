package cards

import (
	"fmt"
	"regexp"

	"ut0311/log"
	"ut0311/scmp"
)

type Cards struct {
	dial struct {
		network string
		address string
	}

	cards []scmp.Card
}

func NewCards(dial string) (*Cards, error) {
	infof("init dial:%v", dial)

	c := Cards{}

	if matches := regexp.MustCompile("(tcp|unix)::(.*)").FindStringSubmatch(dial); len(matches) < 3 {
		return nil, fmt.Errorf("invalid RPC 'dial' address (%v)", dial)
	} else {
		c.dial.network = matches[1]
		c.dial.address = matches[2]
	}

	return &c, nil
}

func (c Cards) GetUint8(oid scmp.OID) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedUint8(oid scmp.OID, index uint32) (uint8, error) {
	if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR1) && index != 17 {
		return 0, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR1) {
		return 1, nil
	}

	if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR2) && index != 17 {
		return 0, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR2) {
		return 1, nil
	}

	if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR3) && index != 17 {
		return 0, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR3) {
		return 0, nil
	}

	if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR4) && index != 17 {
		return 0, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_DOOR4) {
		return 29, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetUint16(oid scmp.OID) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedUint16(oid scmp.OID, index uint32) (uint16, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetUint32(oid scmp.OID) (uint32, error) {
	return 0, fmt.Errorf("** NOT IMPLEMENTED")
}

func (c Cards) GetIndexedUint32(oid scmp.OID, index uint32) (uint32, error) {
	if scmp.Is(oid, scmp.OID_CARDS_CARD_NUMBER) && index != 17 {
		return 0, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_NUMBER) {
		return 10058400, nil
	}

	if scmp.Is(oid, scmp.OID_CARDS_CARD_PIN) && index != 17 {
		return 0, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_PIN) {
		return 54321, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedRecord(oid scmp.OID, index uint32) (any, error) {
	return nil, fmt.Errorf("** NOT IMPLEMENTED")
}

func (c Cards) GetBool(oid scmp.OID) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedBool(oid scmp.OID, index uint32) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetString(oid scmp.OID) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedString(oid scmp.OID, index uint32) (string, error) {
	if scmp.Is(oid, scmp.OID_CARDS_CARD_START_DATE) && index != 17 {
		return "", nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_START_DATE) {
		return "2024-01-01", nil
	}

	if scmp.Is(oid, scmp.OID_CARDS_CARD_END_DATE) && index != 17 {
		return "", nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD_END_DATE) {
		return "2024-12-31", nil
	}

	return "", fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetOctets(oid scmp.OID) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedOctets(oid scmp.OID, index uint32) ([]byte, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetUint8(oid scmp.OID, val uint8) (uint8, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetUint32(oid scmp.OID, val uint32) (uint32, error) {
	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetBool(oid scmp.OID, val bool) (bool, error) {
	return false, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetString(oid scmp.OID, value string) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetUint32A(oid scmp.OID, value []uint32) ([]uint32, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetIndexedRecord(oid scmp.OID, index uint32, value any) (any, error) {
	return nil, fmt.Errorf("** NOT IMPLEMENTED **")
}

func (c *Cards) DeleteIndexedRecord(oid scmp.OID, index uint32) (bool, error) {
	return false, fmt.Errorf("** NOT IMPLEMENTED **")
}

func debugf(format string, args ...any) {
	log.Debugf("SYS", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("SYS", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("SYS", format, args...)
}

func errorf(format string, args ...any) {
	log.Errorf("SYS", format, args...)
}
