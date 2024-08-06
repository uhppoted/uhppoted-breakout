package ssmp

import (
	"bytes"
	"encoding/binary"
	"fmt"
)

const SYN byte = 22
const SOH byte = 1
const STX byte = 2
const ETX byte = 3
const ENQ byte = 5
const ACK byte = 6

var SYN_SYN_ACK = []byte{SYN, SYN, ACK}

func encode(msgid uint32, packet []byte) ([]byte, error) {
	var b bytes.Buffer

	if err := b.WriteByte(SYN); err != nil {
		return nil, err
	}

	if err := b.WriteByte(SYN); err != nil {
		return nil, err
	}

	if err := b.WriteByte(SOH); err != nil {
		return nil, err
	}

	if err := binary.Write(&b, binary.BigEndian, msgid); err != nil {
		return nil, err
	}

	if err := b.WriteByte(STX); err != nil {
		return nil, err
	}

	for ix := 0; ix < len(packet); {
		if N, err := b.Write(packet[ix:]); err != nil {
			return nil, err
		} else if N <= 0 {
			return nil, fmt.Errorf("bisync: encoded %v bytes", N)
		} else {
			ix += N
		}
	}

	if err := b.WriteByte(ETX); err != nil {
		return nil, err
	}

	return b.Bytes(), nil
}
