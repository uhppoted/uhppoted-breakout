package cards

import (
	"fmt"
	"slices"

	"github.com/uhppoted/uhppote-core/types"

	"emulator/log"
	"emulator/scmp"
)

type Cards struct {
	cards []scmp.Card
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
	if scmp.Is(oid, scmp.OID_CARDS_COUNT) {
		return 29, nil
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c Cards) GetIndexedUint32(oid scmp.OID, index uint32) (uint32, error) {
	if scmp.Is(oid, scmp.OID_CARDS_CARD) && index != 10058400 {
		return 0, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_CARD) {
		return 17, nil
	}

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
	if scmp.Is(oid, scmp.OID_CARDS_CARD) && index == 10058400 {
		start, _ := types.ParseDate("2025-01-01")
		end, _ := types.ParseDate("2025-12-31")

		return scmp.Card{
			Card:      1058400,
			StartDate: start,
			EndDate:   end,
			Permissions: map[uint8]uint8{
				1: 1,
				2: 1,
				3: 0,
				4: 29,
			},
			PIN: 97531,
		}, nil
	} else if scmp.Is(oid, scmp.OID_CARDS_INDEX) && index == 17 {
		start, _ := types.ParseDate("2025-01-01")
		end, _ := types.ParseDate("2025-12-31")

		return scmp.Card{
			Card:      1058400,
			StartDate: start,
			EndDate:   end,
			Permissions: map[uint8]uint8{
				1: 1,
				2: 1,
				3: 0,
				4: 29,
			},
			PIN: 97531,
		}, nil
	}

	return nil, fmt.Errorf("unknown OID %v", oid)
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

func (c *Cards) SetString(oid scmp.OID, value string) (string, error) {
	return "", fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetUint32A(oid scmp.OID, value []uint32) ([]uint32, error) {
	return nil, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) SetIndexedRecord(oid scmp.OID, index uint32, value any) (any, error) {
	if scmp.Is(oid, scmp.OID_CARDS_CARD) {
		if card, ok := value.(scmp.Card); ok {
			clone := scmp.Card{
				Card:      card.Card,
				StartDate: card.StartDate,
				EndDate:   card.EndDate,
				Permissions: map[uint8]uint8{
					1: card.Permissions[1],
					2: card.Permissions[2],
					3: card.Permissions[3],
					4: card.Permissions[4],
				},
				PIN: card.PIN,
			}

			for i, v := range c.cards {
				if v.Card == index {
					c.cards[i] = clone

					return clone, nil
				}
			}

			c.cards = append(c.cards, clone)

			return clone, nil
		}
	}

	return 0, fmt.Errorf("unknown OID %v", oid)
}

func (c *Cards) DeleteIndexedRecord(oid scmp.OID, index uint32) (bool, error) {
	if scmp.Is(oid, scmp.OID_CARDS_CARD) {
		deleted := false
		if index > 0 {
			for ix, card := range c.cards {
				if card.Card == index {
					c.cards = slices.Delete(c.cards, ix, ix+1)
					deleted = true
				}
			}
		}

		return deleted, nil
	}

	return false, fmt.Errorf("unknown OID %v", oid)
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
