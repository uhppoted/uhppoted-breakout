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
const CRC_SIZE = 2

var Enq = []byte{SYN, SYN, ENQ}
var Ack = []byte{SYN, SYN, ACK}

type Bisync struct {
	SOH bool
	STX bool
	DLE bool
	CRC bool

	header bytes.Buffer
	packet bytes.Buffer
	crc    bytes.Buffer
}

type Callback interface {
	OnENQ()
	OnACK()
	OnMessage(header []uint8, content []uint8)
}

func NewBisync() *Bisync {
	return &Bisync{
		SOH: false,
		STX: false,
		DLE: false,
		CRC: false,

		header: bytes.Buffer{},
		packet: bytes.Buffer{},
		crc:    bytes.Buffer{},
	}
}

func Encode(header []uint8, packet []uint8) ([]byte, error) {
	var buffer bytes.Buffer

	// ... preamble
	if err := buffer.WriteByte(SYN); err != nil {
		return nil, err
	}

	if err := buffer.WriteByte(SYN); err != nil {
		return nil, err
	}

	// ... encode message header
	if header != nil {
		if err := buffer.WriteByte(SOH); err != nil {
			return nil, err
		}

		for _, b := range header {
			switch b {
			case SYN,
				ENQ,
				ACK,
				SOH,
				STX,
				ETX,
				DLE:
				if err := buffer.WriteByte(DLE); err != nil {
					return nil, err
				}
			}

			if err := buffer.WriteByte(b); err != nil {
				return nil, err
			}
		}
	}

	// ... encode message
	if err := buffer.WriteByte(STX); err != nil {
		return nil, err
	}

	for _, b := range packet {
		switch b {
		case SYN,
			ENQ,
			ACK,
			SOH,
			STX,
			ETX,
			DLE:
			if err := buffer.WriteByte(DLE); err != nil {
				return nil, err
			}

			if err := buffer.WriteByte(b); err != nil {
				return nil, err
			}

		default:
			if err := buffer.WriteByte(b); err != nil {
				return nil, err
			}
		}
	}

	if err := buffer.WriteByte(ETX); err != nil {
		return nil, err
	}

	// ... append CRC
	var crc = uint16(0xffff)

	crc = CRC16(crc, header)
	crc = CRC16(crc, []uint8{STX})
	crc = CRC16(crc, packet)
	crc = CRC16(crc, []uint8{ETX})

	if err := buffer.WriteByte(uint8((crc >> 8) & 0x00ff)); err != nil {
		return nil, err
	}

	if err := buffer.WriteByte(uint8((crc >> 0) & 0x00ff)); err != nil {
		return nil, err
	}

	return buffer.Bytes(), nil
}

func (codec *Bisync) Decode(msg []uint8, f Callback) error {
	if err := codec.decode(msg, f); err != nil {
		codec.reset()
		return err
	}

	return nil
}

func (codec *Bisync) decode(msg []uint8, f Callback) error {
	for _, b := range msg {
		// ... CRC?
		if codec.CRC {
			if err := codec.crc.WriteByte(b); err != nil {
				return err
			}

			if codec.crc.Len() >= 2 {
				crc := uint16(0x0000)

				crc <<= 8
				crc |= uint16(codec.crc.Bytes()[0])
				crc <<= 8
				crc |= uint16(codec.crc.Bytes()[1])

				CRC := uint16(0xffff)
				CRC = CRC16(CRC, codec.header.Bytes())
				CRC = CRC16(CRC, []uint8{STX})
				CRC = CRC16(CRC, codec.packet.Bytes())
				CRC = CRC16(CRC, []uint8{ETX})

				if (CRC ^ crc) == 0x0000 {
					if f != nil {
						f.OnMessage(codec.header.Bytes(), codec.packet.Bytes())
					}
				}

				codec.reset()
			}

			continue
		}

		// ... escaped?
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
			continue
		}

		// ... normal'ish
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
			codec.STX = false
			codec.CRC = true

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

	return nil
}

func (codec *Bisync) reset() {
	codec.SOH = false
	codec.STX = false
	codec.DLE = false
	codec.CRC = false

	codec.header.Reset()
	codec.packet.Reset()
	codec.crc.Reset()
}
