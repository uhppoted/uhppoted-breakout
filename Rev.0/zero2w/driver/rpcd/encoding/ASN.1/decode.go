package BER

import (
	"fmt"
)

func Decode(bytes []byte) (any, error) {
	if v, _, err := unpack(bytes); err != nil {
		return nil, err
	} else if v == nil {
		return nil, nil
	} else if seq, ok := v.(sequence); !ok {
		return nil, fmt.Errorf("invalid packet")
	} else if len(seq) < 3 {
		return nil, fmt.Errorf("invalid packet")
	} else if version, ok := seq[0].(int64); !ok {
		return nil, fmt.Errorf("invalid packet version")
	} else if community, ok := seq[1].(string); !ok {
		return nil, fmt.Errorf("invalid packet community")
	} else if PDU, ok := seq[2].(pdu); !ok || len(PDU) < 4 {
		return nil, fmt.Errorf("invalid packet PDU")
	} else if requestID, ok := PDU[0].(int64); !ok {
		return nil, fmt.Errorf("invalid packet request ID")
	} else if errCode, ok := PDU[1].(int64); !ok {
		return nil, fmt.Errorf("invalid packet error field")
	} else if errIndex, ok := PDU[2].(int64); !ok {
		return nil, fmt.Errorf("invalid packet error index")
	} else if varbindlist, ok := PDU[3].(sequence); !ok || len(varbindlist) < 1 {
		return nil, fmt.Errorf("invalid packet variable bind list")
	} else if vars, ok := varbindlist[0].(sequence); !ok || len(vars) < 2 {
		return nil, fmt.Errorf("invalid packet variable list")
	} else if oid, ok := vars[0].(OID); !ok {
		return nil, fmt.Errorf("invalid packet variable OID")
	} else if _, ok := vars[1].(null); !ok {
		return nil, fmt.Errorf("invalid packet variable value")
	} else {
		return GetRequest{
			Version:    uint8(version),
			Community:  community,
			RequestID:  uint32(requestID),
			Error:      errCode,
			ErrorIndex: errIndex,
			OID:        oid,
		}, nil
	}
}

func unpack(bytes []byte) (any, []byte, error) {
	if len(bytes) == 0 {
		return nil, bytes, nil
	}

	for len(bytes) > 0 {
		tag := bytes[0]
		bytes = bytes[1:]

		switch tag {
		case tagInteger:
			return unpack_integer(bytes)

		case tagOctetString:
			return unpack_octets(bytes)

		case tagNull:
			return unpack_null(bytes)

		case tagOID:
			return unpack_OID(bytes)

		case tagPDU:
			return unpack_PDU(bytes)

		case tagSequence:
			return unpack_sequence(bytes)

		default:
			return nil, bytes, fmt.Errorf("unknown tag (%v)", tag)
		}
	}

	return nil, bytes, fmt.Errorf("invalid packet")
}

func unpack_length(bytes []byte) (uint32, []byte, error) {
	if len(bytes) > 0 {
		b := bytes[0]
		bytes = bytes[1:]

		if (b & 0x80) == 0x00 {
			return uint32(b & 0x7f), bytes, nil
		} else {
			length := uint32(0)
			N := uint32(b & 0x7f)
			bytes = bytes[1:]
			for ; N > 0 && len(bytes) > 0; N-- {
				length <<= 8
				length += (uint32)(bytes[0])
				bytes = bytes[1:]
			}

			if N == 0 {
				return length, bytes, nil
			}
		}
	}

	return 0, bytes, fmt.Errorf("invalid field length")
}

func unpack_integer(bytes []byte) (int64, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return 0, nil, err
	} else if N > uint32(len(bytes)) || N > 8 {
		return 0, nil, fmt.Errorf("invalid integer field")
	} else {
		value := int64(0)
		chunk := bytes[:N]

		if len(chunk) > 0 {
			u8 := uint8(chunk[0])
			chunk = chunk[1:]

			if (u8 & 0x80) == 0x80 {
				value = -1
			} else {
				value = 0
			}

			value <<= 8
			value |= int64(u8) & 0x00ff
		}

		for len(chunk) > 0 {
			u8 := uint8(chunk[0])
			chunk = chunk[1:]

			value <<= 8
			value |= int64(u8) & 0x00ff
		}

		return value, bytes[N:], nil
	}
}

func unpack_octets(bytes []byte) (string, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return "", nil, err
	} else if N > uint32(len(bytes)) {
		return "", nil, fmt.Errorf("invalid octets field")
	} else {
		return string(bytes[0:N]), bytes[N:], nil
	}
}

func unpack_null(bytes []byte) (null, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return null{}, nil, err
	} else if N > uint32(len(bytes)) {
		return null{}, nil, fmt.Errorf("invalid null field")
	} else {
		return null{}, bytes[N:], nil
	}
}

func unpack_OID(bytes []byte) (OID, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return nil, nil, err
	} else if N > uint32(len(bytes)) {
		return nil, nil, err
	} else {
		oid := []uint32{0}
		chunk := bytes[:N]

		if len(chunk) > 0 {
			oid = append(oid, uint32(chunk[0]/40))
			oid = append(oid, uint32(chunk[0]%40))
			chunk = chunk[1:]
		}

		for len(chunk) > 0 {
			v := uint32(0)

			for len(chunk) > 0 {
				b := chunk[0]
				chunk = chunk[1:]

				v <<= 7
				v += (uint32)(b & 0x7f)

				if (b & 0x80) == 0x00 {
					oid = append(oid, v)
					break
				}
			}
		}

		return OID(oid), bytes[N:], nil
	}
}

func unpack_PDU(bytes []byte) (pdu, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return nil, bytes, err
	} else if N > uint32(len(bytes)) {
		return nil, nil, fmt.Errorf("invalid PDU")
	} else {
		PDU := pdu{}
		chunk := bytes[:N]

		for len(chunk) > 0 {
			if field, remaining, err := unpack(chunk); err != nil {
				return nil, bytes[N:], err
			} else {
				PDU = append(PDU, field)
				chunk = remaining
			}
		}

		return PDU, bytes[N:], nil
	}
}

func unpack_sequence(bytes []byte) (sequence, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return nil, nil, err
	} else if N > uint32(len(bytes)) {
		return nil, nil, fmt.Errorf("invalid sequence field")
	} else {
		seq := sequence{}
		chunk := bytes[:N]

		for len(chunk) > 0 {
			if field, remaining, err := unpack(chunk); err != nil {
				return nil, nil, err
			} else {
				seq = append(seq, field)
				chunk = remaining
			}
		}

		return seq, bytes[N:], nil
	}
}
