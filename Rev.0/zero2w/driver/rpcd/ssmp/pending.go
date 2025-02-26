package ssmp

import (
	"sync"
	"time"

	"ssmp/encoding/ASN.1"
)

type pending struct {
	m     map[uint32]e
	guard sync.RWMutex
}

type e struct {
	f       func(packet BER.GetResponse)
	touched time.Time
}

func (p *pending) put(id uint32, f func(packet BER.GetResponse)) {
	p.guard.Lock()
	defer p.guard.Unlock()

	p.m[id] = e{
		f:       f,
		touched: time.Now(),
	}
}

func (p *pending) delete(id uint32) {
	p.guard.Lock()
	defer p.guard.Unlock()

	delete(p.m, id)
}

func (p *pending) get(id uint32) func(packet BER.GetResponse) {
	p.guard.RLock()
	defer p.guard.RUnlock()

	if v, ok := p.m[id]; ok {
		return v.f
	}

	return nil
}

func (p *pending) sweep() {
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
