package UT0311

import (
	"errors"
	"io"
	"net"
	"net/netip"
	"os"
	"sync"
	"time"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"
)

const TCP_READ_TIMEOUT = 30000 * time.Millisecond

type TCP struct {
	socket      *net.TCPListener
	wg          sync.WaitGroup
	cm          *ConnectionManager
	readTimeout time.Duration
	closing     bool
}

func newTCP(cm *ConnectionManager) *TCP {
	return &TCP{
		cm:          cm,
		readTimeout: TCP_READ_TIMEOUT,
	}
}

func (c TCP) isClosing() bool {
	return c.closing
}

func (c *TCP) listen(received func(any) (any, error)) error {
	bind := netip.MustParseAddrPort("0.0.0.0:60000")

	c.wg.Add(1)

	defer func() {
		c.wg.Done()
	}()

	if socket, err := net.ListenTCP("tcp4", net.TCPAddrFromAddrPort(bind)); err != nil {
		return err
	} else {
		infof("listening on TCP address %v", bind)

		defer socket.Close()

		c.socket = socket

		for {
			if client, err := socket.Accept(); err != nil {
				return err
			} else if err := c.cm.add(client); err != nil {
				warnf("TCP  %v", err)
				client.Close()
			} else {
				infof("TCP  incoming")

				go func() {
					if err := c.read(client, received); err != nil && !c.closing {
						warnf("TCP read error (%v)", err)
					}

					c.cm.remove(client)
				}()
			}
		}
	}
}

func (c TCP) read(socket net.Conn, received func(any) (any, error)) error {
	defer func() {
		socket.Close()
		debugf("TCP  closed connection to %v", socket.RemoteAddr())
	}()

	for {
		buffer := make([]byte, 2048)
		deadline := time.Now().Add(c.readTimeout)

		socket.SetReadDeadline(deadline)

		if N, err := socket.Read(buffer); err != nil && errors.Is(err, io.EOF) {
			return nil
		} else if err != nil && errors.Is(err, os.ErrDeadlineExceeded) {
			warnf("TCP  closing connection to %v (idle)", socket.RemoteAddr())
			return nil
		} else if err != nil {
			return err
		} else if N == 0 {
			return nil
		} else {
			debugf("TCP  received %v bytes from %v", N, socket.RemoteAddr())

			if request, err := messages.UnmarshalRequest(buffer[0:N]); err != nil {
				warnf("TCP  %v", err)
			} else {
				go func() {
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
				}()
			}
		}
	}

	return nil
}

func (c *TCP) stop() error {
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
