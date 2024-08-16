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

type callback interface {
	OnENQ()
	OnACK()
	OnMessage(header []uint8, content []uint8)
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

func (codec Bisync) Encode(header []uint8, packet []uint8) ([]byte, error) {
	var b bytes.Buffer

	// ... preamble
	if err := b.WriteByte(SYN); err != nil {
		return nil, err
	}

	if err := b.WriteByte(SYN); err != nil {
		return nil, err
	}

	// ... encode message header
	if header != nil {
		if err := b.WriteByte(SOH); err != nil {
			return nil, err
		}

		for _, byte := range header {
			switch byte {
			case SYN,
				ENQ,
				ACK,
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

	for ix := 0; ix < len(packet); ix++ {
		byte := packet[ix]
		switch byte {
		case SYN,
			ENQ,
			ACK,
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

func (codec *Bisync) Decode(msg []uint8, f callback) error {
	if err := codec.decode(msg, f); err != nil {
		codec.reset()
		return err
	}

	return nil
}

func (codec *Bisync) decode(msg []uint8, f callback) error {
	for _, b := range msg {
		if codec.DLE {
			if codec.SOH {
				if err := codec.header.WriteByte(b); err != nil {
					return err
				}
			}

			if codec.STX {
				if err := codec.packet.WriteByte(b); err != nil {
					return err
				}
			}

			codec.DLE = false

		} else {
			switch b {
			case SYN:
				codec.reset()

			case SOH:
				codec.SOH = true
				codec.STX = false

			case ENQ:
				if codec.packet.Len() == 0 {
					if f != nil {
						f.OnENQ()
					}
				}

				codec.reset()

			case ACK:
				if codec.packet.Len() == 0 {
					if f != nil {
						f.OnACK()
					}
				}

				codec.reset()

			case STX:
				codec.SOH = false
				codec.STX = true

			case ETX:
				codec.SOH = false
				codec.STX = false

				if f != nil {
					f.OnMessage(codec.header.Bytes(), codec.packet.Bytes())
				}

				codec.reset()

			case DLE:
				codec.DLE = true

			default:
				if codec.SOH {
					if codec.header.Len() < HEADER_SIZE {
						if err := codec.header.WriteByte(b); err != nil {
							return err
						}
					} else {
						return fmt.Errorf("header overflow")
					}
				}

				if codec.STX {
					if codec.packet.Len() < PACKET_SIZE {
						if err := codec.packet.WriteByte(b); err != nil {
							return err
						}
					} else {
						return fmt.Errorf("buffer overflow")
					}
				}
			}
		}
	}

	return nil
}

func (codec *Bisync) reset() {
	codec.SOH = false
	codec.STX = false
	codec.DLE = false

	codec.header.Reset()
	codec.packet.Reset()
}
