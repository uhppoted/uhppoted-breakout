package UT0311

import (
	"crypto/tls"
	"crypto/x509"
	"errors"
	"fmt"
	"io"
	"net"
	"os"
	"sync"
	"time"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"
)

type TLS struct {
	Certificate string
	CA          string

	socket  net.Listener
	wg      sync.WaitGroup
	closing bool
}

func makeTLS(certificate string, ca string) *TLS {
	return &TLS{
		Certificate: certificate,
		CA:          ca,
	}
}

func (c TLS) isClosing() bool {
	return c.closing
}

func (c *TLS) listen(received func(any) (any, error)) error {
	bind := "0.0.0.0:60443"
	certificates := x509.NewCertPool()

	c.wg.Add(1)

	defer func() {
		c.wg.Done()
	}()

	if certificate, err := tls.LoadX509KeyPair(c.Certificate, ".key"); err != nil {
		return err
	} else if CA, err := os.ReadFile(c.CA); err != nil {
		return err
	} else if !certificates.AppendCertsFromPEM(CA) {
		return fmt.Errorf("error parsing CA/client certificates")
	} else {

		config := &tls.Config{
			Certificates: []tls.Certificate{certificate},
			ClientAuth:   tls.RequireAndVerifyClientCert,
			ClientCAs:    certificates,
		}

		if socket, err := tls.Listen("tcp4", bind, config); err != nil {
			return err
		} else {
			infof("listening on TLS address %v", bind)

			defer socket.Close()

			c.socket = socket

			for {
				if client, err := socket.Accept(); err != nil {
					return err
				} else {
					infof("TLS  incoming")

					go func() {
						if err := c.read(client, received); err != nil {
							warnf("TLS read error (%v)", err)
						}
					}()
				}
			}
		}
	}
}

func (c TLS) read(socket net.Conn, received func(any) (any, error)) error {
	defer func() {
		socket.Close()
		debugf("TLS  closed connection to %v", socket.RemoteAddr())
	}()

	for {
		buffer := make([]byte, 2048)
		deadline := time.Now().Add(READ_TIMEOUT)

		socket.SetReadDeadline(deadline)

		if N, err := socket.Read(buffer); err != nil && errors.Is(err, io.EOF) {
			return nil
		} else if err != nil && errors.Is(err, os.ErrDeadlineExceeded) {
			warnf("TLS  closing connection to %v (idle)", socket.RemoteAddr())
			return nil
		} else if err != nil {
			return err
		} else if N == 0 {
			return nil
		} else {
			debugf("TLS  received %v bytes from %v", N, socket.RemoteAddr())

			if request, err := messages.UnmarshalRequest(buffer[0:N]); err != nil {
				warnf("TLS  %v", err)
			} else {
				reply, err := received(request)
				if err != nil {
					warnf("TLS  %v", err)
				}

				if !isnil(reply) {
					if packet, err := codec.Marshal(reply); err != nil {
						warnf("TLS  %v", err)
					} else if packet == nil {
						warnf("TLS  invalid reply packet (%v)", packet)
					} else if N, err := socket.Write(packet); err != nil {
						warnf("TLS  %v", err)
					} else {
						debugf("TLS  sent %v bytes to %v", N, socket.RemoteAddr())
					}
				}
			}
		}

	}

	return nil
}

func (c *TLS) stop() error {
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
