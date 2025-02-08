package bisync

import (
	"bytes"
)

const SOH uint8 = 1
const STX uint8 = 2
const ETX uint8 = 3
const ENQ uint8 = 5
const ACK uint8 = 6
const DLE uint8 = 16
const NAK uint8 = 21
const SYN uint8 = 22

func Encode(header []uint8, packet []byte) ([]byte, error) {
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
