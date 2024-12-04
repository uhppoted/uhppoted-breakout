package rpcd

import (
	"fmt"
	"log"
	"net"
	"net/http"
	"net/rpc"
)

type RPCD int

func (r *RPCD) Get(oid string, reply *any) error {
	// ... controller ID
	if oid == ".1.3.6.1.4.1.65536.2.1" {
		*reply = uint32(405419896)

		return nil
	}

	// ... controller version
	if oid == ".1.3.6.1.4.1.65536.2.2" {
		*reply = uint16(0x1234)

		return nil
	}

	// ... controller release date
	if oid == ".1.3.6.1.4.1.65536.2.3" {
		*reply = "2024-01-15"

		return nil
	}

	return fmt.Errorf("unknown OID %v", oid)
}

func Run() {
	rpcd := new(RPCD)

	rpc.Register(rpcd)
	rpc.HandleHTTP()

	if l, err := net.Listen("tcp", ":1234"); err != nil {
		log.Fatal("listen error:", err)
	} else {
		http.Serve(l, nil)
	}
}
