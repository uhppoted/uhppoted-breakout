package rpcd

import (
	"fmt"
	"maps"
	"sync"
	"time"

	"ut0311/scmp"
)

type entry struct {
	value   any
	expires time.Time
}

type cache struct {
	cache     sync.Map
	cacheable map[string]time.Duration
}

func NewCache(m map[string]time.Duration) *cache {
	return &cache{
		cache:     sync.Map{},
		cacheable: maps.Clone(m),
	}
}

func (c cache) Get(oid scmp.OID) (any, bool) {
	k := fmt.Sprintf("%v", oid)

	if v, ok := c.cache.Load(k); ok {
		if e, ok := v.(entry); ok && e.expires.After(time.Now()) {
			return e.value, true
		}
	}

	return nil, false
}

func (c *cache) Set(oid scmp.OID, value any) {
	k := fmt.Sprintf("%v", oid)

	if lifetime, ok := c.cacheable[k]; ok {
		v := entry{
			value:   value,
			expires: time.Now().Add(lifetime),
		}

		c.cache.Store(k, v)
	}
}
