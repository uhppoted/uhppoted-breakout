package main

import (
	"flag"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"logging/logger"
)

const VERSION = "v0.0"

var options = struct {
	config string
	device string
}{
	config: ".config",
	device: "",
}

func main() {
	fmt.Printf("uhppoted-breakout::logging %v\n", VERSION)

	flag.StringVar(&options.config, "config", options.config, ".config file")
	flag.StringVar(&options.device, "device", options.device, "serial device")
	flag.Parse()

	if options.device == "" {
		fmt.Printf("  *** ERROR  missing serial device")
		os.Exit(1)
	} else {
		go func() {
			logger.Logger{
				Device: options.device,
			}.Run()
		}()

		interrupt := make(chan os.Signal, 1)

		signal.Notify(interrupt, syscall.SIGINT, syscall.SIGTERM)

		<-interrupt
		fmt.Printf("  ... interrupted\n")
	}

}
