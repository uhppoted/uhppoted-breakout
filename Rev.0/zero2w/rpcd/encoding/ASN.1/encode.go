package BER

import (
	"bytes"
	"slices"
)

func EncodeGetRequest(rq GetRequest) ([]byte, error) {
	packet := [][]byte{}

	if v, err := pack_integer(int64(rq.Version)); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	if v, err := pack_octets(rq.Community); err != nil {
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

	// ... error
	if v, err := pack_integer(0); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	// ... error index
	if v, err := pack_integer(0); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	varbindlist := [][]byte{}
	varbind := [][]byte{}

	if v, err := pack_oid(rq.OID); err != nil {
		return nil, err
	} else {
		varbind = append(varbind, v)
	}

	if v, err := pack_null(); err != nil {
		return nil, err
	} else {
		varbind = append(varbind, v)
	}

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

	if v, err := pack_pdu(tagGetRequest, pdu); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	return pack_sequence(packet...)
}

func EncodeGetResponse(response GetResponse) ([]byte, error) {
	packet := [][]byte{}

	if v, err := pack_integer(int64(response.Version)); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	if v, err := pack_octets(response.Community); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	pdu := [][]byte{}

	if v, err := pack_integer(int64(response.RequestID)); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	if v, err := pack_integer(response.Error); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	if v, err := pack_integer(response.ErrorIndex); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	varbindlist := [][]byte{}
	varbind := [][]byte{}

	if v, err := pack_oid(response.OID); err != nil {
		return nil, err
	} else {
		varbind = append(varbind, v)
	}

	// ... value
	switch val := response.Value.(type) {
	case uint32:
		if v, err := pack_integer(int64(val)); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	case uint16:
		if v, err := pack_integer(int64(val)); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	case string:
		if v, err := pack_octets(string(val)); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	default:
		if v, err := pack_null(); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}
	}

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

	if v, err := pack_pdu(tagGetResponse, pdu); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	return pack_sequence(packet...)
}

func EncodeSetRequest(rq SetRequest) ([]byte, error) {
	packet := [][]byte{}

	if v, err := pack_integer(int64(rq.Version)); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	if v, err := pack_octets(rq.Community); err != nil {
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

	// ... error
	if v, err := pack_integer(0); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	// ... error index
	if v, err := pack_integer(0); err != nil {
		return nil, err
	} else {
		pdu = append(pdu, v)
	}

	varbindlist := [][]byte{}
	varbind := [][]byte{}

	if v, err := pack_oid(rq.OID); err != nil {
		return nil, err
	} else {
		varbind = append(varbind, v)
	}

	// ... value
	switch val := rq.Value.(type) {
	case bool:
		if v, err := pack_bool(val); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	case uint8:
		if v, err := pack_integer(int64(val)); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	case uint16:
		if v, err := pack_integer(int64(val)); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	case uint32:
		if v, err := pack_integer(int64(val)); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	case string:
		if v, err := pack_octets(string(val)); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}

	default:
		if v, err := pack_null(); err != nil {
			return nil, err
		} else {
			varbind = append(varbind, v)
		}
	}

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

	if v, err := pack_pdu(tagSetRequest, pdu); err != nil {
		return nil, err
	} else {
		packet = append(packet, v)
	}

	return pack_sequence(packet...)
}

func pack_bool(bval bool) ([]byte, error) {
	var b bytes.Buffer
	var u8 uint8 = 0

	if bval {
		u8 = 1
	}

	if err := b.WriteByte(tagBoolean); err != nil {
		return nil, err
	}

	if l, err := pack_varuint(uint32(1)); err != nil {
		return nil, err
	} else if _, err := b.Write(l); err != nil {
		return nil, err
	}

	if err := b.WriteByte(u8); err != nil {
		return nil, err
	}

	return b.Bytes(), nil
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

func pack_octets(s string) ([]byte, error) {
	var b bytes.Buffer

	if err := b.WriteByte(tagOctetString); err != nil {
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

func pack_null() ([]byte, error) {
	return []byte{tagNull, 0}, nil
}

func pack_oid(oid OID) ([]byte, error) {
	var b bytes.Buffer
	var ix = 0

	// ... skip root '0'
	if ix < len(oid) && oid[ix] == 0 {
		ix++
	}

	// ... special encoding for first byte (.1.3)
	if ix < len(oid) {
		v := uint8(40 * oid[ix])
		ix++

		if ix < len(oid) {
			v += uint8(oid[ix])
			ix++
		}

		if err := b.WriteByte(v); err != nil {
			return nil, err
		}
	}

	// ... remaining sub-OIDs
	for ix < len(oid) {
		v := oid[ix]

		if slice, err := pack_varuint(v); err != nil {
			return nil, err
		} else if _, err := b.Write(slice); err != nil {
			return nil, err
		}

		ix++
	}

	// ... copy to slice

	if length, err := pack_varuint(uint32(b.Len())); err != nil {
		return nil, err
	} else {
		slice := append([]byte{tagOID}, length...)
		slice = append(slice, b.Bytes()...)

		return slice, nil
	}
}

func pack_pdu(tag byte, pdu [][]byte) ([]byte, error) {
	var b bytes.Buffer

	var length uint32 = 0
	for _, v := range pdu {
		length += uint32(len(v))
	}

	if err := b.WriteByte(tag); err != nil {
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
func pack_varuint(u32 uint32) ([]byte, error) {
	var b bytes.Buffer

	v := u32

	if err := b.WriteByte(byte(v & 0x7f)); err != nil {
		return nil, err
	} else {
		v >>= 7
	}

	for v != 0 {
		if err := b.WriteByte(0x80 | uint8(v&0x7f)); err != nil {
			return nil, err
		} else {
			v >>= 7
		}
	}

	// ... reverse
	slice := b.Bytes()

	slices.Reverse(slice)
	return slice, nil
}
