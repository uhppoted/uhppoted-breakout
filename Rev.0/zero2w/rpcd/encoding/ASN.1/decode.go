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
		return nil, fmt.Errorf("invalid packet - expected SEQUENCE")
	} else if len(seq) < 3 {
		return nil, fmt.Errorf("invalid packet - expected PDU")
	} else if version, ok := seq[0].(int64); !ok {
		return nil, fmt.Errorf("invalid packet version")
	} else if community, ok := seq[1].(string); !ok {
		return nil, fmt.Errorf("invalid packet community")
	} else if PDU, ok := seq[2].(pdu); ok {
		return decode_PDU(version, community, PDU)
	} else if TRAP, ok := seq[2].(trap); ok {
		return decode_TRAP(version, community, TRAP)
	} else {
		return nil, fmt.Errorf("unknown PDU type (%v)", PDU.tag)
	}
}

func decode_PDU(version int64, community string, PDU pdu) (any, error) {
	if len(PDU.vars) < 1 {
		return nil, fmt.Errorf("invalid packet PDU")
	} else if vars := PDU.vars; len(vars) < 1 {
		return nil, fmt.Errorf("invalid packet variable bind list")
	} else {
		v := vars[0]

		switch PDU.tag {
		case tagGetRequest:
			return GetRequest{
				Version:   uint8(version),
				Community: community,
				RequestID: PDU.requestId,
				OID:       v.OID,
			}, nil

		case tagGetResponse:
			return GetResponse{
				Version:    uint8(version),
				Community:  community,
				RequestID:  PDU.requestId,
				Error:      PDU.errorCode,
				ErrorIndex: PDU.errorIndex,
				OID:        v.OID,
				Value:      v.Value,
			}, nil

		case tagSetRequest:
			return SetRequest{
				Version:   uint8(version),
				Community: community,
				RequestID: PDU.requestId,
				OID:       v.OID,
				Value:     v.Value,
			}, nil

		default:
			return nil, fmt.Errorf("unknown PDU type (%v)", PDU.tag)
		}
	}
}

func decode_TRAP(version int64, community string, TRAP trap) (any, error) {
	return Trap{
		Version:   uint8(version),
		Community: community,
		OID:       TRAP.oid,
		ID:        TRAP.id,
		Category:  TRAP.category,
		Event:     TRAP.event,
		Timestamp: TRAP.timestamp,
		Vars:      TRAP.vars,
	}, nil
}

func unpack(bytes []byte) (any, []byte, error) {
	if len(bytes) == 0 {
		return nil, bytes, nil
	}

	for len(bytes) > 0 {
		tag := bytes[0]
		bytes = bytes[1:]

		switch tag {
		case tagBoolean:
			return unpack_boolean(bytes)

		case tagInteger:
			return unpack_integer(bytes)

		case tagOctetString:
			return unpack_octets(bytes)

		case tagNull:
			return unpack_null(bytes)

		case tagOID:
			return unpack_OID(bytes)

		case tagGetRequest:
			return unpack_PDU(tagGetRequest, bytes)

		case tagGetResponse:
			return unpack_PDU(tagGetResponse, bytes)

		case tagSetRequest:
			return unpack_PDU(tagSetRequest, bytes)

		case tagTrap:
			return unpack_TRAP(tagTrap, bytes)

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

func unpack_boolean(bytes []byte) (bool, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return false, nil, err
	} else if N > uint32(len(bytes)) || N > 8 {
		return false, nil, fmt.Errorf("invalid boolean field")
	} else {
		chunk := bytes[:N]

		if len(chunk) > 0 {
			u8 := uint8(chunk[0])

			if u8 == 0x00 {
				return false, bytes[N:], nil
			} else if u8 == 0xff {
				return true, bytes[N:], nil
			}

			return false, nil, fmt.Errorf("invalid boolean value")
		}

		return false, nil, fmt.Errorf("invalid boolean field")
	}
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

func unpack_PDU(tag byte, bytes []byte) (pdu, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return pdu{}, bytes, err
	} else if N > uint32(len(bytes)) {
		return pdu{}, nil, fmt.Errorf("invalid PDU")
	} else {
		PDU := pdu{
			tag: tag,
		}
		chunk := bytes[:N]

		if len(chunk) > 0 {
			if v, remaining, err := unpack(chunk); err != nil {
				return pdu{}, bytes[N:], err
			} else if u32, ok := v.(int64); !ok {
				return pdu{}, nil, fmt.Errorf("invalid packet request ID")
			} else {
				PDU.requestId = uint32(u32)
				chunk = remaining
			}

			if len(chunk) > 0 {
				if v, remaining, err := unpack(chunk); err != nil {
					return pdu{}, bytes[N:], err
				} else if i64, ok := v.(int64); !ok {
					return pdu{}, nil, fmt.Errorf("invalid packet error code")
				} else {
					PDU.errorCode = i64
					chunk = remaining
				}
			}

			if len(chunk) > 0 {
				if v, remaining, err := unpack(chunk); err != nil {
					return pdu{}, bytes[N:], err
				} else if i64, ok := v.(int64); !ok {
					return pdu{}, nil, fmt.Errorf("invalid packet error index")
				} else {
					PDU.errorIndex = i64
					chunk = remaining
				}
			}

			if len(chunk) > 0 {
				if v, remaining, err := unpack(chunk); err != nil {
					return pdu{}, bytes[N:], err
				} else if seq, ok := v.(sequence); !ok {
					return pdu{}, nil, fmt.Errorf("invalid packet varbindlist")
				} else {
					chunk = remaining

					for _, e := range seq {
						if v, ok := e.(sequence); !ok || len(v) < 2 {
							return pdu{}, nil, fmt.Errorf("invalid packet var")
						} else if oid, ok := v[0].(OID); !ok {
							return pdu{}, nil, fmt.Errorf("invalid packet variable OID")
						} else {
							PDU.vars = append(PDU.vars, variable{
								OID:   oid,
								Value: v[1],
							})
						}
					}
				}
			}

		}

		return PDU, bytes[N:], nil
	}
}

func unpack_TRAP(tag byte, bytes []byte) (trap, []byte, error) {
	if N, bytes, err := unpack_length(bytes); err != nil {
		return trap{}, bytes, err
	} else if N > uint32(len(bytes)) {
		return trap{}, nil, fmt.Errorf("invalid PDU")
	} else {
		TRAP := trap{
			tag: tag,
		}
		chunk := bytes[:N]

		if len(chunk) > 0 {
			if v, remaining, err := unpack(chunk); err != nil {
				return trap{}, bytes[N:], err
			} else if oid, ok := v.(OID); !ok {
				return trap{}, nil, fmt.Errorf("invalid trap OID")
			} else {
				TRAP.oid = oid
				chunk = remaining
			}

			if v, remaining, err := unpack(chunk); err != nil {
				return trap{}, bytes[N:], err
			} else if u32, ok := v.(int64); !ok {
				return trap{}, nil, fmt.Errorf("invalid trap origin ID")
			} else {
				TRAP.id = uint32(u32)
				chunk = remaining
			}

			if v, remaining, err := unpack(chunk); err != nil {
				return trap{}, bytes[N:], err
			} else if u32, ok := v.(int64); !ok {
				return trap{}, nil, fmt.Errorf("invalid trap category")
			} else {
				TRAP.category = uint32(u32)
				chunk = remaining
			}

			if v, remaining, err := unpack(chunk); err != nil {
				return trap{}, bytes[N:], err
			} else if u32, ok := v.(int64); !ok {
				return trap{}, nil, fmt.Errorf("invalid trap event")
			} else {
				TRAP.event = uint32(u32)
				chunk = remaining
			}

			if v, remaining, err := unpack(chunk); err != nil {
				return trap{}, bytes[N:], err
			} else if timestamp, ok := v.(string); !ok {
				return trap{}, nil, fmt.Errorf("invalid trap timestamp")
			} else {
				TRAP.timestamp = timestamp
				chunk = remaining
			}

			if v, remaining, err := unpack(chunk); err != nil {
				return trap{}, bytes[N:], err
			} else if vars, ok := v.(sequence); !ok {
				return trap{}, nil, fmt.Errorf("invalid trap vars list")
			} else {
				for _, vv := range vars {
					if seq, ok := vv.(sequence); !ok || len(seq) != 2 {
						return trap{}, nil, fmt.Errorf("invalid trap var (%v) <ok:%v, len:%v>", seq, ok, len(seq))
					} else if oid, ok := seq[0].(OID); !ok {
						return trap{}, nil, fmt.Errorf("invalid trap var (%v)", seq)
					} else {
						TRAP.vars = append(TRAP.vars, variable{oid, seq[1]})
					}
				}

				chunk = remaining
			}
		}

		return TRAP, bytes[N:], nil
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
