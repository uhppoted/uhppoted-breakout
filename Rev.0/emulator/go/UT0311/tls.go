package UT0311

import (
	"crypto/tls"
	"crypto/x509"
	"errors"
	"fmt"
	"io"
	"net"
	"os"
	"time"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"
)

type TLS struct {
}

func (tcp TLS) listen(received func(any) (any, error)) error {
	bind := "0.0.0.0:60443"
	certificates := x509.NewCertPool()

	if certificate, err := tls.LoadX509KeyPair(".certificate", ".key"); err != nil {
		return err
	} else if CA, err := os.ReadFile(".CA"); err != nil {
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

			for {
				if client, err := socket.Accept(); err != nil {
					warnf("TLS  accept error (%v)", err)
				} else {
					infof("TLS  incoming")

					go func() {
						if err := tcp.read(client, received); err != nil {
							warnf("TLS read error (%v)", err)
						}
					}()
				}
			}
		}
	}
}

func (tcp TLS) read(socket net.Conn, received func(any) (any, error)) error {
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
