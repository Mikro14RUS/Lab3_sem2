package containers

import (
	"strconv"
	"testing"
)

func BenchmarkArrayPushBack(b *testing.B) {
	values := NewArray("bench")
	for i := 0; i < b.N; i++ {
		values.PushBack("value_" + strconv.Itoa(i))
	}
}

func BenchmarkHashTablePut(b *testing.B) {
	table := NewHashTable("bench", 16)
	for i := 0; i < b.N; i++ {
		table.Put("key_"+strconv.Itoa(i), strconv.Itoa(i))
	}
}

func BenchmarkHashTableGet(b *testing.B) {
	table := NewHashTable("bench", 16)
	const size = 10000
	for i := 0; i < size; i++ {
		table.Put("key_"+strconv.Itoa(i), strconv.Itoa(i))
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		table.Get("key_" + strconv.Itoa(i%size))
	}
}

func BenchmarkFullBinaryTreeInsert(b *testing.B) {
	tree := NewFullBinaryTree("bench")
	state := uint64(20261108)
	for i := 0; i < b.N; i++ {
		state ^= state << 13
		state ^= state >> 7
		state ^= state << 17
		tree.Insert(int(state % 1000000000))
	}
}

func BenchmarkQueuePushPop(b *testing.B) {
	queue := NewQueue("bench")
	for i := 0; i < b.N; i++ {
		queue.Push("value")
		queue.Pop()
	}
}
