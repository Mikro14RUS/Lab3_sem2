package containers

import (
	"encoding/json"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// TestArrayWithTestify демонстрирует стиль assert/require, требуемый
// лабораторной работой. Те же сценарии есть и в стандартном наборе
// testing; этот файл явно проверяет работу с testify.
func TestArrayWithTestify(t *testing.T) {
	a := NewArray("arr")
	require.True(t, a.InsertAt(0, "b"))
	require.True(t, a.InsertAt(0, "a"))
	require.True(t, a.InsertAt(2, "c"))
	assert.Equal(t, []string{"a", "b", "c"}, a.Values())
	assert.Equal(t, "[a, b, c]", a.String())

	value, ok := a.GetAt(1)
	require.True(t, ok)
	assert.Equal(t, "b", value)
	assert.False(t, a.SetAt(99, "bad"))

	payload, err := json.Marshal(a)
	require.NoError(t, err)
	var restored Array
	require.NoError(t, json.Unmarshal(payload, &restored))
	assert.Equal(t, a.Values(), restored.Values())
	assert.Error(t, json.Unmarshal([]byte(`"not-an-array"`), &restored))
}

func TestContainersWithTestify(t *testing.T) {
	stack := NewStack("s")
	stack.Push("one")
	stack.Push("two")
	value, ok := stack.Peek()
	require.True(t, ok)
	assert.Equal(t, "two", value)
	value, ok = stack.Pop()
	require.True(t, ok)
	assert.Equal(t, "two", value)

	queue := NewQueue("q")
	queue.Push("first")
	queue.Push("second")
	front, ok := queue.Front()
	require.True(t, ok)
	assert.Equal(t, "first", front)

	table := NewHashTable("h", 2)
	assert.True(t, table.Put("key", "value"))
	assert.False(t, table.Put("key", "updated"))
	value, ok = table.Get("key")
	require.True(t, ok)
	assert.Equal(t, "updated", value)
	assert.True(t, table.Contains("key"))
	removed, ok := table.Remove("key")
	require.True(t, ok)
	assert.Equal(t, "updated", removed)

	tree := NewFullBinaryTree("t")
	assert.True(t, tree.Insert(50))
	assert.True(t, tree.Insert(30))
	assert.True(t, tree.Insert(70))
	assert.False(t, tree.Insert(30))
	assert.True(t, tree.IsFull())
	assert.True(t, tree.IsBST())
	assert.Equal(t, []int{30, 50, 70}, tree.Inorder())
}
