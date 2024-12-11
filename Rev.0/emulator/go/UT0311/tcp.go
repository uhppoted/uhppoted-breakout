package UT0311

import (
	"net"
	"net/netip"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"
)

type TCP struct {
}

func (tcp TCP) listen(received func(any) (any, error)) error {
	bind := netip.MustParseAddrPort("0.0.0.0:60000")

	if socket, err := net.ListenTCP("tcp4", net.TCPAddrFromAddrPort(bind)); err != nil {
		return err
	} else {
		infof("listening on TCP address %v", bind)

		defer socket.Close()

		for {
			if client, err := socket.Accept(); err != nil {
				warnf("TCP accept error (%v)", err)
			} else {
				infof("TCP incoming")

				go func() {
					buffer := make([]byte, 2048)

					if N, err := client.Read(buffer); err != nil {
						warnf("TCP read error (%v)", err)
					} else {
						debugf("TCP  received %v bytes from %v", N, client.RemoteAddr())

						if request, err := messages.UnmarshalRequest(buffer[0:N]); err != nil {
							warnf("TCP  %v", err)
						} else {
							reply, err := received(request)

							if err != nil {
								warnf("TCP  %v", err)
							}

							if !isnil(reply) {
								if packet, err := codec.Marshal(reply); err != nil {
									warnf("TCP  %v", err)
								} else if packet == nil {
									warnf("TCP  invalid reply packet (%v)", packet)
								} else if N, err := client.Write(packet); err != nil {
									warnf("TCP  %v", err)
								} else {
									debugf("TCP  sent %v bytes to %v", N, client.RemoteAddr())
								}
							}
						}
					}

					if err := client.Close(); err != nil {
						warnf("TCP close error (%v)", err)
					}
				}()
			}
		}
	}
}
