package rpcd

import (
	"log"
	"net"
	"net/http"
	"net/rpc"
)

type RPCD int

func (r *RPCD) Get(oid string, reply *uint32) error {
	*reply = 405419896

	return nil
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
