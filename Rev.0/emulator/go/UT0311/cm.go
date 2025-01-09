package UT0311

import (
	"fmt"
	"net"
	"sync"
)

const MAX_CONNECTIONS = 5

type ConnectionManager struct {
	guard sync.Mutex
	count int
	max   int
}

func NewConnectionManager(max int) *ConnectionManager {
	cm := ConnectionManager{
		count: 0,
		max:   max,
	}

	return &cm
}

func (cm *ConnectionManager) add(connection net.Conn) error {
	cm.guard.Lock()
	defer cm.guard.Unlock()

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
