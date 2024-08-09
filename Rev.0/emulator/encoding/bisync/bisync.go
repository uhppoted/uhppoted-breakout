package bisync

import (
	"bytes"
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
	SOH bool
	STX bool
	DLE bool
}

type Message struct {
	Header []byte
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
	if len(msg.Header) > 0 {
		if err := b.WriteByte(SOH); err != nil {
			return nil, err
		}

		for _, byte := range msg.Header {
			switch byte {
			case SYN,
				SOH,
				STX,
				ETX,
				DLE:
				if err := b.WriteByte(DLE); err != nil {
					return nil, err
				}
			}

			if err := b.WriteByte(byte); err != nil {
				return nil, err
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
	messages := []Message{}
	header := bytes.Buffer{}
	packet := bytes.Buffer{}

	for _, b := range msg {
		if codec.DLE {
			if codec.SOH {
				if err := header.WriteByte(b); err != nil {
					return messages, err
				}
			}

			if codec.STX {
				if err := packet.WriteByte(b); err != nil {
					return messages, err
				}
			}

			codec.DLE = false

		} else if b == SOH {
			codec.SOH = true
			codec.STX = false
		} else if b == STX {
			codec.SOH = false
			codec.STX = true
		} else if b == ETX {
			codec.SOH = false
			codec.STX = false

			messages = append(messages, Message{
				Header: header.Bytes(),
				Packet: packet.Bytes(),
			})
		} else if b == DLE {
			codec.DLE = true
		} else if codec.SOH {
			if err := header.WriteByte(b); err != nil {
				return messages, err
			}
		} else if codec.STX {
			if err := packet.WriteByte(b); err != nil {
				return messages, err
			}
		}
	}

	return messages, nil
}
