package bisync

import (
	"bytes"
	"fmt"
)

const SOH byte = 1
const STX byte = 2
const ETX byte = 3
const ENQ byte = 5
const ACK byte = 6
const DLE byte = 16
const SYN byte = 22

const HEADER_SIZE = 128
const PACKET_SIZE = 512

var Enq = []byte{SYN, SYN, ENQ}
var Ack = []byte{SYN, SYN, ACK}

type Bisync struct {
	SOH bool
	STX bool
	DLE bool

	header bytes.Buffer
	packet bytes.Buffer
}

type Message struct {
	Header []byte
	Packet []byte
}

func NewBisync() *Bisync {
	return &Bisync{
		SOH:    false,
		STX:    false,
		DLE:    false,
		header: bytes.Buffer{},
		packet: bytes.Buffer{},
	}
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
	if msg.Header != nil {
		if err := b.WriteByte(SOH); err != nil {
			return nil, err
		}

		for _, byte := range msg.Header {
			switch byte {
			case SYN,
				ENQ,
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
			ENQ,
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
	messages, err := codec.decode(msg)
	if err != nil {
		codec.reset()
	}

	return messages, err
}

func (codec *Bisync) decode(msg []uint8) ([]Message, error) {
	messages := []Message{}

	for _, b := range msg {
		if codec.DLE {
			if codec.SOH {
				if err := codec.header.WriteByte(b); err != nil {
					return messages, err
				}
			}

			if codec.STX {
				if err := codec.packet.WriteByte(b); err != nil {
					return messages, err
				}
			}

			codec.DLE = false

		} else {
			switch b {
			case SOH:
				codec.SOH = true
				codec.STX = false

			case STX:
				codec.SOH = false
				codec.STX = true

			case ETX:
				codec.SOH = false
				codec.STX = false

				messages = append(messages, Message{
					Header: codec.header.Bytes(),
					Packet: codec.packet.Bytes(),
				})

				codec.reset()

			case DLE:
				codec.DLE = true

			default:
				if codec.SOH {
					if codec.header.Len() < HEADER_SIZE {
						if err := codec.header.WriteByte(b); err != nil {
							return messages, err
						}
					} else {
						return messages, fmt.Errorf("header overflow")
					}
				}

				if codec.STX {
					if codec.packet.Len() < PACKET_SIZE {
						if err := codec.packet.WriteByte(b); err != nil {
							return messages, err
						}
					} else {
						return messages, fmt.Errorf("buffer overflow")
					}
				}
			}
		}
	}

	return messages, nil
}

func (codec *Bisync) reset() {
	codec.SOH = false
	codec.STX = false
	codec.DLE = false

	codec.header.Reset()
	codec.packet.Reset()
}
