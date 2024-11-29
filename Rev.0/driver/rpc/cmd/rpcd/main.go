package main

import (
	"fmt"

	"ssmp/rpcd"
)

const VERSION = "v0.0.0"

func main() {
	fmt.Printf("SSMP RPC DRIVER %v\n", VERSION)

	rpcd.Run()
}
