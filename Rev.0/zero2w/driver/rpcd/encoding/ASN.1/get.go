package BER

type GetRequest struct {
	Version    uint8
	Community  string
	RequestID  uint32
	Error      int64
	ErrorIndex int64
	OID        OID
}
