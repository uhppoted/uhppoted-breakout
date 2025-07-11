package UT0311

import (
	"net"
	"net/netip"
	"sync"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"
)

type UDP struct {
	socket  *net.UDPConn
	wg      sync.WaitGroup
	cm      *ConnectionManager
	closing bool
}

func newUDP(cm *ConnectionManager) *UDP {
	return &UDP{
		cm: cm,
	}
}

func (c UDP) isClosing() bool {
	return c.closing
}

func (c *UDP) listen(received func(any) (any, error)) error {
	bind := netip.MustParseAddrPort("0.0.0.0:60000")

	c.wg.Add(1)

	defer func() {
		c.wg.Done()
	}()

	if socket, err := net.ListenUDP("udp4", net.UDPAddrFromAddrPort(bind)); err != nil {
		return err
	} else {
		infof("listening on UDP address %v", bind)

		defer socket.Close()

		c.socket = socket

		for {
			buffer := make([]byte, 2048)

			if N, addr, err := socket.ReadFromUDPAddrPort(buffer); err != nil {
				return err
			} else {
				debugf("UDP  received %v bytes from %v", N, addr)

				if request, err := messages.UnmarshalRequest(buffer[0:N]); err != nil {
					warnf("UDP  %v", err)
				} else {
					go func() {
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
					}()
				}
			}
		}
	}
}

func (udp *UDP) sendto(dest netip.AddrPort, message any) {
	addr := net.UDPAddrFromAddrPort(dest)
	msg, err := codec.Marshal(message)

	if err != nil {
		errorf("%v", err)
		return
	}

	if socket, err := net.DialUDP("udp4", nil, addr); err != nil {
		errorf("UDP", "error dialing UDP listener [%v]", err)
	} else {

		defer socket.Close()

		if N, err := socket.Write(msg); err != nil {
			errorf("failed to write message to UDP socket [%v]", err)
		} else {
			infof("sent %v bytes to %v", N, dest)
		}
	}
}

func (c *UDP) stop() error {
	c.closing = true

	if c.socket != nil {
		if err := c.socket.Close(); err != nil {
			return err
		} else {
			c.wg.Wait()
		}
	}

	return nil
}
