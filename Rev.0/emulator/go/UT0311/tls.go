package UT0311

import (
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"os"

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
					warnf("TLS accept error (%v)", err)
				} else {
					infof("TLS incoming")

					go func() {
						buffer := make([]byte, 2048)

						if N, err := client.Read(buffer); err != nil {
							warnf("TLS read error (%v)", err)
						} else {
							debugf("TLS  received %v bytes from %v", N, client.RemoteAddr())

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
									} else if N, err := client.Write(packet); err != nil {
										warnf("TLS  %v", err)
									} else {
										debugf("TLS  sent %v bytes to %v", N, client.RemoteAddr())
									}
								}
							}
						}

						if err := client.Close(); err != nil {
							warnf("TLS close error (%v)", err)
						}
					}()
				}
			}
		}
	}
}
