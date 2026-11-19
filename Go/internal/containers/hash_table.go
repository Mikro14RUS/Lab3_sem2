package containers

import (
	"encoding/json"
	"fmt"
	"sort"
	"strings"
)

type hashEntry struct {
	Key   string `json:"key"`
	Value string `json:"value"`
}

// HashTable — хеш-таблица с разрешением коллизий методом цепочек.
// Используется FNV-1a и автоматическое расширение при load factor > 0.75.
type HashTable struct {
	name       string
	buckets    [][]hashEntry
	size       int
	collisions int
}

func NewHashTable(name string, initialBuckets int) *HashTable {
	if initialBuckets < 2 {
		initialBuckets = 2
	}
	return &HashTable{name: name, buckets: make([][]hashEntry, initialBuckets)}
}

func (h *HashTable) Type() string { return TypeHash }
func (h *HashTable) Name() string { return h.name }
func (h *HashTable) Len() int     { return h.size }

func (h *HashTable) Clear() {
	for i := range h.buckets {
		h.buckets[i] = nil
	}
	h.size = 0
	h.collisions = 0
}

func (h *HashTable) Put(key, value string) bool {
	index := h.bucketIndex(key)
	for i := range h.buckets[index] {
		if h.buckets[index][i].Key == key {
			h.buckets[index][i].Value = value
			return false
		}
	}
	if len(h.buckets[index]) > 0 {
		h.collisions++
	}
	h.buckets[index] = append(h.buckets[index], hashEntry{Key: key, Value: value})
	h.size++
	if h.size*4 > len(h.buckets)*3 {
		h.rehash(len(h.buckets) * 2)
	}
	return true
}

func (h *HashTable) Get(key string) (string, bool) {
	for _, entry := range h.buckets[h.bucketIndex(key)] {
		if entry.Key == key {
			return entry.Value, true
		}
	}
	return "", false
}

func (h *HashTable) Remove(key string) (string, bool) {
	index := h.bucketIndex(key)
	bucket := h.buckets[index]
	for i, entry := range bucket {
		if entry.Key == key {
			h.buckets[index] = append(bucket[:i], bucket[i+1:]...)
			h.size--
			return entry.Value, true
		}
	}
	return "", false
}

func (h *HashTable) Contains(key string) bool {
	_, ok := h.Get(key)
	return ok
}

func (h *HashTable) Entries() []hashEntry {
	result := make([]hashEntry, 0, h.size)
	for _, bucket := range h.buckets {
		result = append(result, bucket...)
	}
	sort.Slice(result, func(i, j int) bool { return result[i].Key < result[j].Key })
	return result
}

func (h *HashTable) BucketCount() int { return len(h.buckets) }
func (h *HashTable) Collisions() int  { return h.collisions }

func (h *HashTable) LoadFactor() float64 {
	if len(h.buckets) == 0 {
		return 0
	}
	return float64(h.size) / float64(len(h.buckets))
}

func (h *HashTable) String() string {
	parts := make([]string, 0, h.size)
	for _, entry := range h.Entries() {
		parts = append(parts, entry.Key+"="+entry.Value)
	}
	return "{" + strings.Join(parts, ", ") + "}"
}

func (h *HashTable) bucketIndex(key string) int {
	return int(fnv1a(key) % uint64(len(h.buckets)))
}

func (h *HashTable) rehash(newCapacity int) {
	if newCapacity < 2 {
		newCapacity = 2
	}
	old := h.buckets
	h.buckets = make([][]hashEntry, newCapacity)
	h.size = 0
	h.collisions = 0
	for _, bucket := range old {
		for _, entry := range bucket {
			h.Put(entry.Key, entry.Value)
		}
	}
}

func fnv1a(value string) uint64 {
	hash := uint64(1469598103934665603)
	for i := 0; i < len(value); i++ {
		hash ^= uint64(value[i])
		hash *= 1099511628211
	}
	return hash
}

func (h *HashTable) MarshalJSON() ([]byte, error) {
	return json.Marshal(h.Entries())
}

func (h *HashTable) UnmarshalJSON(data []byte) error {
	var entries []hashEntry
	if err := json.Unmarshal(data, &entries); err != nil {
		return fmt.Errorf("hash: %w", err)
	}
	h.Clear()
	for _, entry := range entries {
		h.Put(entry.Key, entry.Value)
	}
	return nil
}
