package UT0311

import (
	"net"
	"net/netip"
	"reflect"
	"time"

	codec "github.com/uhppoted/uhppote-core/encoding/UTO311-L0x"
	"github.com/uhppoted/uhppote-core/messages"

	"emulator/config"
	"emulator/driver"
	"emulator/log"
)

type UT0311 struct {
	config config.Config
	driver driver.Driver
}

func (ut0311 *UT0311) SetConfig(c config.Config) {
	ut0311.config = c
}

func NewUT0311(c config.Config, d driver.Driver) UT0311 {
	return UT0311{
		config: c,
		driver: d,
	}
}

func (ut0311 *UT0311) Run() {
	for {
		if err := ut0311.listen(); err != nil {
			warnf("%v", err)
		}

		// TODO: exponential backoff
		time.Sleep(5 * time.Second)
	}
}

// func (ut0311 *UT0311) initialise(config Config) {
// 	var address netip.Addr
// 	var netmask net.IPMask
// 	var gateway netip.Addr
// 	var MAC net.HardwareAddr
//
// 	if v := config.Network.IPv4; v != nil {
// 		address = v.Address
// 		netmask = v.Netmask
// 		gateway = v.Gateway
// 		MAC = v.MAC
// 	}
//
// 	if v := config.Network.Interface; v != "" {
// 		if addr, subnet, gw, mac, err := discover(v); err != nil {
// 			warnf("%v", err)
// 		} else {
// 			address = addr
// 			netmask = subnet
// 			gateway = gw
// 			MAC = mac
// 		}
// 	}
//
// 	if err := MIB.Init(address, netmask, gateway, MAC); err != nil {
// 		warnf("%v", err)
// 	}
// }

func (ut0311 UT0311) listen() error {
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
				} else if reply, err := ut0311.received(request); err != nil {
					warnf("UDP  %v", err)
				} else if !isnil(reply) {
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

func (ut0311 UT0311) received(request any) (any, error) {
	infof("UDP  request %T", request)

	switch rq := request.(type) {
	case *messages.GetDeviceRequest:
		return ut0311.GetController(rq)

	default:
		warnf("unknown message type (%T)", request)
	}

	return nil, nil
}

func isnil(v any) bool {
	if v == nil {
		return true
	}

	switch reflect.TypeOf(v).Kind() {
	case reflect.Ptr,
		reflect.Map,
		reflect.Array,
		reflect.Chan,
		reflect.Slice:
		return reflect.ValueOf(v).IsNil()
	}

	return false
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

func errorf(format string, args ...any) {
	log.Errorf("UT0311", format, args...)
}
