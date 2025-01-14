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
	guard       sync.Mutex
	max         int
	closed      bool
	rate        *rate.Limiter
	connections map[net.Conn]struct{}
}

func NewConnectionManager(max int) *ConnectionManager {
	cm := ConnectionManager{
		max:         max,
		closed:      false,
		rate:        rate.NewLimiter(CONNECTION_RATE_LIMIT, CONNECTION_BURST_LIMIT),
		connections: map[net.Conn]struct{}{},
	}

	return &cm
}

func (cm *ConnectionManager) add(connection net.Conn) error {
	cm.guard.Lock()
	defer cm.guard.Unlock()

	if cm.closed {
		return fmt.Errorf("closed")
	}

	if !cm.rate.Allow() {
		return fmt.Errorf("connection rate limit exceeded")
	}

	if len(cm.connections) < cm.max {
		cm.connections[connection] = struct{}{}
		return nil
	}

	return fmt.Errorf("too many concurrent connections")
}

func (cm *ConnectionManager) remove(connection net.Conn) {
	cm.guard.Lock()
	defer cm.guard.Unlock()

	delete(cm.connections, connection)
}

func (cm *ConnectionManager) close() error {
	cm.guard.Lock()
	defer cm.guard.Unlock()

	cm.closed = true

	for c, _ := range cm.connections {
		if err := c.Close(); err != nil {
			warnf("error closing managed connection (%v)", err)
		}
	}

	cm.connections = map[net.Conn]struct{}{}

	return nil
}
