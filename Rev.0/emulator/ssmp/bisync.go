package ssmp

import (
	"bytes"
	"encoding/binary"
	"fmt"
)

const SOH byte = 1
const STX byte = 2
const ETX byte = 3
const ENQ byte = 5
const ACK byte = 6
const DLE byte = 16
const SYN byte = 22

var SYN_SYN_ACK = []byte{SYN, SYN, ACK}

func encode(msgid uint32, packet []byte) ([]byte, error) {
	var b bytes.Buffer

	// ... preamble
	if err := b.WriteByte(SYN); err != nil {
		return nil, err
	}

	if err := b.WriteByte(SYN); err != nil {
		return nil, err
	}

	// ... encode message header
	if err := b.WriteByte(SOH); err != nil {
		return nil, err
	}

	{
		buffer := make([]byte, 4)
		binary.BigEndian.PutUint32(buffer, msgid)

		for _, byte := range buffer {
			switch byte {
			case SYN,
				SOH,
				STX,
				ETX,
				DLE:
				if err := b.WriteByte(DLE); err != nil {
					return nil, err
				}

				if err := b.WriteByte(byte); err != nil {
					return nil, err
				}

			default:
				if err := b.WriteByte(byte); err != nil {
					return nil, err
				}
			}
		}
	}

	// ... encode message
	if err := b.WriteByte(STX); err != nil {
		return nil, err
	}

	for ix := 0; ix < len(packet); ix++ {
		byte := packet[ix]
		switch byte {
		case SYN,
			SOH,
			STX,
			ETX,
			DLE:
			if err := b.WriteByte(DLE); err != nil {
				return nil, err
			}

			if err := b.WriteByte(byte); err != nil {
				return nil, err
			}

		default:
			if err := b.WriteByte(byte); err != nil {
				return nil, err
			}
		}
	}

	if err := b.WriteByte(ETX); err != nil {
		return nil, err
	}

	return b.Bytes(), nil
}

func decode(msg []uint8) (uint32, []byte, error) {
	packet := bytes.Buffer{}
	msgid := uint32(0)
	ix := 0

	// .. get message header
	for ix < len(msg) {
		if b := msg[ix]; b == SOH {
			break
		} else {
			ix++
		}
	}

	if msg[ix] != SOH {
		return 0, nil, fmt.Errorf("missing SOH")
	} else {
		ix++
	}

	for i := 0; i < 4; i++ {
		if ix < len(msg) {
			b := msg[ix]
			ix++

			msgid <<= 8
			msgid += uint32(b)
		} else {
			return 0, nil, fmt.Errorf("missing message ID")
		}
	}

	// ... get message content
	for ix < len(msg) {
		if b := msg[ix]; b == STX {
			break
		} else {
			ix++
		}
	}

	if msg[ix] != STX {
		return 0, nil, fmt.Errorf("missing STX")
	} else {
		ix++
	}

	for ix < len(msg) {
		if b := msg[ix]; b == ETX {
			break
		} else if err := packet.WriteByte(b); err != nil {
			return 0, nil, err
		} else {
			ix++
		}
	}

	if msg[ix] != ETX {
		return 0, nil, fmt.Errorf("missing ETX")
	} else {
		ix++
	}

	return msgid, packet.Bytes(), nil
}
