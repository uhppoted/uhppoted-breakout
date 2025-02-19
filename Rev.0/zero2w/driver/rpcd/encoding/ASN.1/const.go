package BER

const tagInteger byte = 2
const tagOctetString byte = 4
const tagNull byte = 5
const tagOID byte = 6
const tagSequence byte = 48
const tagGetRequest byte = 160
const tagGetResponse byte = 162

type sequence []any
type pdu []any
type null struct{}
