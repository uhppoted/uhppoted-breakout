package ssmp

import (
	"sync"
	"time"

	"ssmp/encoding/ASN.1"
)

type packet interface {
	BER.GetResponse
}

type pending[T packet] struct {
	m     map[uint32]e[T]
	guard sync.RWMutex
}

type e[T packet] struct {
	f       func(packet T)
	touched time.Time
}

func (p *pending[T]) put(id uint32, f func(packet T)) {
	p.guard.Lock()
	defer p.guard.Unlock()

	p.m[id] = e[T]{
		f:       f,
		touched: time.Now(),
	}
}

func (p *pending[T]) delete(id uint32) {
	p.guard.Lock()
	defer p.guard.Unlock()

	delete(p.m, id)
}

func (p *pending[T]) get(id uint32) func(packet T) {
	p.guard.RLock()
	defer p.guard.RUnlock()

	if v, ok := p.m[id]; ok {
		return v.f
	}

	return nil
}

func (p *pending[T]) sweep() {
	expired := map[uint32]bool{}
	cutoff := time.Now().Add(-15 * time.Second)

	p.guard.RLock()
	for k, v := range p.m {
		if v.touched.Before(cutoff) {
			expired[k] = true
		}
	}
	p.guard.RUnlock()

	if len(expired) > 0 {
		debugf("expired %v", expired)
		p.guard.Lock()
		for k := range expired {
			delete(p.m, k)
		}
		p.guard.Unlock()
	}

}
