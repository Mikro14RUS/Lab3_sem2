package containers

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

func TestSaveLoadText(t *testing.T) {
	array := NewArray("arr")
	array.PushBack("a")
	array.PushBack("b")

	list := NewSinglyLinkedList("list")
	list.PushBack("one")

	dlist := NewDoublyLinkedList("dlist")
	dlist.PushFront("first")

	stack := NewStack("stack")
	stack.Push("top")

	queue := NewQueue("queue")
	queue.Push("front")

	table := NewHashTable("hash", 4)
	table.Put("key", "value")

	tree := NewFullBinaryTree("tree")
	for _, value := range []int{50, 30, 70} {
		require.True(t, tree.Insert(value))
	}

	path := filepath.Join(t.TempDir(), "structures.json")
	items := []Container{array, list, dlist, stack, queue, table, tree}
	require.NoError(t, SaveText(path, items))

	loaded, err := LoadText(path)
	require.NoError(t, err)
	require.Len(t, loaded, len(items))
	byName := make(map[string]Container, len(loaded))
	for _, item := range loaded {
		byName[item.Name()] = item
	}
	assert.Equal(t, TypeArray, byName["arr"].Type())
	assert.Equal(t, "[a, b]", byName["arr"].String())
	assert.Equal(t, []string{"one"}, byName["list"].(*SinglyLinkedList).Values())
	assert.Equal(t, []string{"first"}, byName["dlist"].(*DoublyLinkedList).Values())
	assert.Equal(t, []string{"top"}, byName["stack"].(*Stack).Values())
	assert.Equal(t, []string{"front"}, byName["queue"].(*Queue).Values())
	assert.Equal(t, "value", mustHashValue(t, byName["hash"].(*HashTable), "key"))
	assert.Equal(t, []int{30, 50, 70}, byName["tree"].(*FullBinaryTree).Inorder())
}

func mustHashValue(t *testing.T, table *HashTable, key string) string {
	t.Helper()
	value, ok := table.Get(key)
	require.True(t, ok)
	return value
}

func TestSaveLoadEmpty(t *testing.T) {
	path := filepath.Join(t.TempDir(), "empty.json")
	require.NoError(t, SaveText(path, nil))
	loaded, err := LoadText(path)
	require.NoError(t, err)
	assert.Empty(t, loaded)
}

func TestLoadTextRejectsBadFormat(t *testing.T) {
	path := filepath.Join(t.TempDir(), "bad.json")
	require.NoError(t, os.WriteFile(path, []byte(`{"magic":"OTHER","version":1,"items":[]}`), 0o644))
	_, err := LoadText(path)
	assert.Error(t, err)

	require.NoError(t, os.WriteFile(path, []byte(`{"magic":"DS3TXT","version":99,"items":[]}`), 0o644))
	_, err = LoadText(path)
	assert.Error(t, err)

	require.NoError(t, os.WriteFile(path, []byte(`{`), 0o644))
	_, err = LoadText(path)
	assert.Error(t, err)
}

func TestLoadTextRejectsBadRecords(t *testing.T) {
	path := filepath.Join(t.TempDir(), "bad-records.json")
	data := `{"magic":"DS3TXT","version":1,"items":[
		{"type":"unknown","name":"x","payload":[]},
		{"type":"array","name":"","payload":[]},
		{"type":"array","name":"a","payload":"bad"},
		{"type":"array","name":"a","payload":[]}
	]}`
	require.NoError(t, os.WriteFile(path, []byte(data), 0o644))
	_, err := LoadText(path)
	assert.Error(t, err)
}

func TestSaveTextRejectsEmptyName(t *testing.T) {
	path := filepath.Join(t.TempDir(), "empty-name.json")
	err := SaveText(path, []Container{NewArray("")})
	assert.Error(t, err)
}

func TestLoadTextMissingFile(t *testing.T) {
	_, err := LoadText(filepath.Join(t.TempDir(), "missing.json"))
	assert.Error(t, err)
}
