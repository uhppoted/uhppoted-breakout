package UT0311

import (
	"net"
	"net/netip"
	"time"

	"github.com/uhppoted/uhppoted-breakout/Rev.0/emulator/log"
)

type UT0311 struct {
}

func (ut0311 UT0311) Run() {
	for {
		if err := ut0311.listen(); err != nil {
			warnf("%v", err)
		}

		// TODO: exponential backoff
		time.Sleep(5 * time.Second)
	}
}

func (ut0311 UT0311) listen() error {
	bind := netip.MustParseAddrPort("0.0.0.0:60000")

	if socket, err := net.ListenUDP("udp4", net.UDPAddrFromAddrPort(bind)); err != nil {
		return err
	} else {
		infof("listening on UDP address %v", bind)

		defer socket.Close()

		for {
			request := make([]byte, 2048)

			if N, addr, err := socket.ReadFromUDPAddrPort(request); err != nil {
				return err
			} else {
				debugf("UDP  received %v bytes from %v", N, addr)
			}
		}
	}
}

func debugf(format string, args ...any) {
	log.Debugf("UT0311", format, args...)
}

func infof(format string, args ...any) {
	log.Infof("UT0311", format, args...)
}

func warnf(format string, args ...any) {
	log.Warnf("UT0311", format, args...)
}
