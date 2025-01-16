package main

import (
	"flag"
	"fmt"

	"ssmp/rpcd"
)

const VERSION = "v0.0.0"

var options = struct {
	bind string
}{
	bind: "tcp:::1234",
}

func main() {
	fmt.Printf("SSMP RPC DRIVER %v\n", VERSION)

	flag.StringVar(&options.bind, "bind", options.bind, "bind address (in the format network::address:port e.g. tcp::0.0.0.0:12345")
	flag.Parse()

	rpcd.Run(options.bind)
}
