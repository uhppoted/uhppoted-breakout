package UT0311

import (
	"errors"
	"io"
	"net"
	"net/netip"
	"os"
	"time"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"
)

type TCP struct {
}

const READ_TIMEOUT = 30000 * time.Millisecond

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
					if err := tcp.read(client, received); err != nil {
						warnf("TCP read error (%v)", err)
					}
				}()
			}
		}
	}
}

func (tcp TCP) read(socket net.Conn, received func(any) (any, error)) error {
	defer socket.Close()

	for {
		buffer := make([]byte, 2048)
		deadline := time.Now().Add(READ_TIMEOUT)

		socket.SetReadDeadline(deadline)

		if N, err := socket.Read(buffer); err != nil && errors.Is(err, io.EOF) {
			return nil
		} else if err != nil && errors.Is(err, os.ErrDeadlineExceeded) {
			warnf("TCP  closing connection to %v (idle)", socket.RemoteAddr())
			return nil
		} else if err != nil {
			return err
		} else {
			debugf("TCP  received %v bytes from %v", N, socket.RemoteAddr())

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
					} else if N, err := socket.Write(packet); err != nil {
						warnf("TCP  %v", err)
					} else {
						debugf("TCP  sent %v bytes to %v", N, socket.RemoteAddr())
					}
				}
			}
		}
	}

	return nil
}
