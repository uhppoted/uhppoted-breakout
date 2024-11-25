package SSMP

import (
	syslog "log"
	"os"

	"github.com/gosnmp/gosnmp"
)

type GetPacket struct {
	Version   uint8
	Community string
	RequestID uint32
	OID       string
}

func (p GetPacket) Encode() ([]byte, error) {
	logger := gosnmp.NewLogger(syslog.New(os.Stdout, "", 0))

	pdu := gosnmp.SnmpPDU{
		Value: gosnmp.Null,
		Name:  p.OID,
		Type:  gosnmp.Null,
	}

	packet := gosnmp.SnmpPacket{
		Version:         gosnmp.Version1,
		ContextEngineID: "ssmp",
		ContextName:     "ssmp",
		Community:       p.Community,
		PDUType:         gosnmp.GetRequest,
		MsgID:           1,
		RequestID:       p.RequestID,
		MsgMaxSize:      512,
		Error:           gosnmp.NoError,
		ErrorIndex:      0,
		NonRepeaters:    0,
		MaxRepetitions:  0,
		Variables:       []gosnmp.SnmpPDU{pdu},
		Logger:          logger,
	}

	return packet.MarshalMsg()
}
