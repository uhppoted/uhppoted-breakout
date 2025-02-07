package BER

import (
	"bytes"
	"slices"
)

const tagInteger byte = 2
const tagString byte = 4
const tagSequence byte = 48
const tagPDU byte = 160

func Encode(rq GetRequest) ([]byte, error) {
	packet := [][]byte{}

	if v, err := pack_integer(int64(rq.Version)); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	if v, err := pack_string(rq.Community); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	pdu := [][]byte{}

	if v, err := pack_integer(int64(rq.RequestID)); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	if v, err := pack_integer(rq.Error); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	if v, err := pack_integer(rq.ErrorIndex); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	varbindlist := [][]byte{}
	varbind := [][]byte{}

	if v, err := pack_sequence(varbind...); err != nil {
		return nil, err
	} else {
		varbindlist = append(varbindlist, v)
	}

	if v, err := pack_sequence(varbindlist...); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	if v, err := pack_pdu(pdu); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	return pack_sequence(packet...)
}

func pack_integer(ival int64) ([]byte, error) {
	var b bytes.Buffer

	var v int64 = ival
	var u8 uint8 = 0
	for {
		u8 = uint8(v & 0x00ff)
		if err := b.WriteByte(u8); err != nil {
			return nil, err
		}

		v >>= 8
		if (v == 0) || (v == -1) {
			break
		}
	}

	// ... pad if +ve and most-significant bit is set OR -ve and most-significant bit is not set
	if ival > 0 && u8 > 0x007f {
		if err := b.WriteByte(0); err != nil {
			return nil, err
		}
	} else if ival < 0 && u8 < 0x80 {
		if err := b.WriteByte(0xff); err != nil {
			return nil, err
		}
	}

	// ... length
	if l, err := pack_varuint(uint32(b.Len())); err != nil {
		return nil, err
	} else {
		slices.Reverse(l)

		if _, err := b.Write(l); err != nil {
			return nil, err
		}
	}

	// ... tag
	if err := b.WriteByte(tagInteger); err != nil {
		return nil, err
	}

	slice := b.Bytes()

	slices.Reverse(slice)

	return slice, nil
}

func pack_string(s string) ([]byte, error) {
	var b bytes.Buffer

	if err := b.WriteByte(tagString); err != nil {
		return nil, err
	}

	if l, err := pack_varuint(uint32(len(s))); err != nil {
		return nil, err
	} else if _, err := b.Write(l); err != nil {
		return nil, err
	}

	if _, err := b.Write([]byte(s)); err != nil {
		return nil, err
	}

	return b.Bytes(), nil
}

func pack_pdu(pdu [][]byte) ([]byte, error) {
	var b bytes.Buffer

	var length uint32 = 0
	for _, v := range pdu {
		length += uint32(len(v))
	}

	if err := b.WriteByte(tagPDU); err != nil {
		return nil, err
	}

	if l, err := pack_varuint(length); err != nil {
		return nil, err
	} else if _, err := b.Write(l); err != nil {
		return nil, err
	}

	for _, v := range pdu {
		if _, err := b.Write(v); err != nil {
			return nil, err
		}
	}

	return b.Bytes(), nil
}

func pack_sequence(fields ...[]byte) ([]byte, error) {
	var b bytes.Buffer

	var length uint32 = 0
	for _, v := range fields {
		length += uint32(len(v))
	}

	if err := b.WriteByte(tagSequence); err != nil {
		return nil, err
	}

	if l, err := pack_varuint(length); err != nil {
		return nil, err
	} else if _, err := b.Write(l); err != nil {
		return nil, err
	}

	for _, v := range fields {
		if _, err := b.Write(v); err != nil {
			return nil, err
		}
	}
	return b.Bytes(), nil
}

// Encodes a uint32 varint as a big-endian varint
func pack_varuint(length uint32) ([]byte, error) {
	var b bytes.Buffer

	v := length

	if err := b.WriteByte(byte(v & 0x7f)); err != nil {
		return nil, err
	} else {
		v >>= 7
	}

	// ... reverse
	slice := b.Bytes()

	slices.Reverse(slice)
	return slice, nil
}
