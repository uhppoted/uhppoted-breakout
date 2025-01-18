package BER

import (
	"github.com/gosnmp/gosnmp"
)

func Encode(packet gosnmp.SnmpPacket) ([]byte, error) {
	return packet.MarshalMsg()
}

func Decode(msg []uint8) (*gosnmp.SnmpPacket, error) {
	snmp := gosnmp.GoSNMP{}

	return snmp.SnmpDecodePacket(msg)
}
