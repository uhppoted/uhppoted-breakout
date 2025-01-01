package UT0311

import (
	"net"
	"net/netip"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"
)

type UDP struct {
}

func (c UDP) listen(received func(any) (any, error)) error {
	bind := netip.MustParseAddrPort("0.0.0.0:60000")

	if socket, err := net.ListenUDP("udp4", net.UDPAddrFromAddrPort(bind)); err != nil {
		return err
	} else {
		infof("listening on UDP address %v", bind)

		defer socket.Close()

		for {
			buffer := make([]byte, 2048)

			if N, addr, err := socket.ReadFromUDPAddrPort(buffer); err != nil {
				return err
			} else {
				debugf("UDP  received %v bytes from %v", N, addr)

				if request, err := messages.UnmarshalRequest(buffer[0:N]); err != nil {
					warnf("UDP  %v", err)
				} else {
					reply, err := received(request)

					if err != nil {
						warnf("UDP  %v", err)
					}

					if !isnil(reply) {
						if packet, err := codec.Marshal(reply); err != nil {
							warnf("UDP  %v", err)
						} else if packet == nil {
							warnf("UDP  invalid reply packet (%v)", packet)
						} else if N, err := socket.WriteToUDPAddrPort(packet, addr); err != nil {
							warnf("UDP  %v", err)
						} else {
							debugf("UDP  sent %v bytes to %v", N, addr)
						}
					}
				}
			}
		}
	}
}
