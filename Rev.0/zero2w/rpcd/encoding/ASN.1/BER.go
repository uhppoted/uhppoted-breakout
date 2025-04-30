package BER

const tagBoolean byte = 1
const tagInteger byte = 2
const tagOctetString byte = 4
const tagNull byte = 5
const tagOID byte = 6
const tagSequence byte = 48
const tagGetRequest byte = 160
const tagGetResponse byte = 162
const tagSetRequest byte = 163
const tagTrap byte = 164

type GetRequest struct {
	Version   uint8
	Community string
	RequestID uint32
	OID       OID
}

type GetResponse struct {
	Version    uint8
	Community  string
	RequestID  uint32
	Error      int64
	ErrorIndex int64
	OID        OID
	Value      any
}

type SetRequest struct {
	Version   uint8
	Community string
	RequestID uint32
	OID       OID
	Value     any
}

type Trap struct {
	Version   uint8
	Community string
	OID       OID
	ID        uint32
	Category  uint32
	Event     uint32
	Timestamp string
	// OID OID
	// Value any
}

type sequence []any
type variable struct {
	oid   OID
	value any
}

type pdu struct {
	tag        byte
	requestId  uint32
	errorCode  int64
	errorIndex int64
	vars       []variable
}

type trap struct {
	tag       byte
	oid       OID
	id        uint32
	category  uint32
	event     uint32
	timestamp string
}

type null struct{}
