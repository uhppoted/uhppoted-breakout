package cache

import (
	"maps"
	"sync"
	"time"
)

type entry struct {
	value   any
	expires time.Time
}

var cache = sync.Map{}

var cacheable = map[string]time.Duration{}

func Init(m map[string]time.Duration) {
	cacheable = maps.Clone(m)
}

func Get(oid string) (any, bool) {
	if v, ok := cache.Load(oid); ok {
		if e, ok := v.(entry); ok && e.expires.After(time.Now()) {
			return e.value, true
		}
	}

	return nil, false
}

func Set(oid string, value any) {
	if lifetime, ok := cacheable[oid]; ok {
		v := entry{
			value:   value,
			expires: time.Now().Add(lifetime),
		}

		cache.Store(oid, v)
	}
}
