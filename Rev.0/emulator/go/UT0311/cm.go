package UT0311

import (
	"fmt"
	"net"
	"sync"

	"golang.org/x/time/rate"
)

const MAX_CONNECTIONS = 5
const CONNECTION_RATE_LIMIT = 1.0  // connections/second
const CONNECTION_BURST_LIMIT = 5.0 // burst connection requests

type ConnectionManager struct {
	guard sync.Mutex
	count int
	max   int
	rate  *rate.Limiter
}

func NewConnectionManager(max int) *ConnectionManager {
	cm := ConnectionManager{
		count: 0,
		max:   max,
		rate:  rate.NewLimiter(CONNECTION_RATE_LIMIT, CONNECTION_BURST_LIMIT),
	}

	return &cm
}

func (cm *ConnectionManager) add(connection net.Conn) error {
	cm.guard.Lock()
	defer cm.guard.Unlock()

	if !cm.rate.Allow() {
		return fmt.Errorf("connection rate limit exceeded")
	}

	if cm.count < cm.max {
		cm.count++
		return nil
	}

	return fmt.Errorf("too many concurrent connections")
}

func (cm *ConnectionManager) remove(connection net.Conn) {
	cm.guard.Lock()
	defer cm.guard.Unlock()

	if cm.count > 0 {
		cm.count--
	}
}
