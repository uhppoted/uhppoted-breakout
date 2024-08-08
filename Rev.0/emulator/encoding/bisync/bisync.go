package bisync

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

var Enq = []byte{SYN, SYN, ENQ}
var Ack = []byte{SYN, SYN, ACK}

type Bisync struct {
}

type Message struct {
	Header uint32
	Packet []byte
}

func (codec Bisync) Encode(msg Message) ([]byte, error) {
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
		binary.BigEndian.PutUint32(buffer, msg.Header)

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

	for ix := 0; ix < len(msg.Packet); ix++ {
		byte := msg.Packet[ix]
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

func (codec *Bisync) Decode(msg []uint8) ([]Message, error) {
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
		return nil, fmt.Errorf("missing SOH")
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
			return nil, fmt.Errorf("missing message ID")
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
		return nil, fmt.Errorf("missing STX")
	} else {
		ix++
	}

	for ix < len(msg) {
		if b := msg[ix]; b == ETX {
			break
		} else if err := packet.WriteByte(b); err != nil {
			return nil, err
		} else {
			ix++
		}
	}

	if msg[ix] != ETX {
		return nil, fmt.Errorf("missing ETX")
	} else {
		ix++
	}

	return []Message{
		Message{
			Header: msgid,
			Packet: packet.Bytes(),
		},
	}, nil
}
