package containers

import (
	"bytes"
	"encoding/binary"
	"os"
	"path/filepath"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

type binaryTestRecord struct {
	typeName string
	name     string
	payload  []byte
}

func buildBinaryTestFile(records ...binaryTestRecord) []byte {
	var out bytes.Buffer
	out.WriteString(binaryMagic)
	writeBinaryUint32(&out, binaryVersion)
	writeBinaryUint32(&out, uint32(len(records)))
	for _, record := range records {
		_ = writeBinaryString(&out, record.typeName)
		_ = writeBinaryString(&out, record.name)
		writeBinaryUint32(&out, uint32(len(record.payload)))
		out.Write(record.payload)
	}
	return out.Bytes()
}

func TestSaveLoadBinary(t *testing.T) {
	array := NewArray("arr")
	array.PushBack("a")
	array.PushBack("b")

	list := NewSinglyLinkedList("list")
	list.PushBack("one")

	dlist := NewDoublyLinkedList("dlist")
	dlist.PushFront("first")

	stack := NewStack("stack")
	stack.Push("bottom")
	stack.Push("top")

	queue := NewQueue("queue")
	queue.Push("front")
	queue.Push("back")
	_, ok := queue.Pop()
	require.True(t, ok)
	queue.Push("last")

	table := NewHashTable("hash", 4)
	table.Put("key", "value")
	table.Put("another", "data")

	tree := NewFullBinaryTree("tree")
	for _, value := range []int{50, 30, 70, 20, 40, 60, 80} {
		require.True(t, tree.Insert(value))
	}

	path := filepath.Join(t.TempDir(), "structures.bin")
	items := []Container{array, list, dlist, stack, queue, table, tree}
	require.NoError(t, SaveBinary(path, items))

	data, err := os.ReadFile(path)
	require.NoError(t, err)
	require.GreaterOrEqual(t, len(data), len(binaryMagic))
	assert.Equal(t, binaryMagic, string(data[:len(binaryMagic)]))

	loaded, err := LoadBinary(path)
	require.NoError(t, err)
	require.Len(t, loaded, len(items))
	byName := make(map[string]Container, len(loaded))
	for _, item := range loaded {
		byName[item.Name()] = item
	}

	assert.Equal(t, TypeArray, byName["arr"].Type())
	assert.Equal(t, []string{"a", "b"}, byName["arr"].(*Array).Values())
	assert.Equal(t, TypeList, byName["list"].Type())
	assert.Equal(t, []string{"one"}, byName["list"].(*SinglyLinkedList).Values())
	assert.Equal(t, TypeDList, byName["dlist"].Type())
	assert.Equal(t, []string{"first"}, byName["dlist"].(*DoublyLinkedList).Values())
	assert.Equal(t, TypeStack, byName["stack"].Type())
	assert.Equal(t, []string{"top", "bottom"}, byName["stack"].(*Stack).Values())
	assert.Equal(t, TypeQueue, byName["queue"].Type())
	assert.Equal(t, []string{"back", "last"}, byName["queue"].(*Queue).Values())
	assert.Equal(t, TypeHash, byName["hash"].Type())
	assert.Equal(t, "value", mustHashValue(t, byName["hash"].(*HashTable), "key"))
	assert.Equal(t, "data", mustHashValue(t, byName["hash"].(*HashTable), "another"))
	assert.Equal(t, TypeTree, byName["tree"].Type())
	assert.Equal(t, []int{20, 30, 40, 50, 60, 70, 80},
		byName["tree"].(*FullBinaryTree).Inorder())
	assert.True(t, byName["tree"].(*FullBinaryTree).IsFull())
	assert.True(t, byName["tree"].(*FullBinaryTree).IsBST())
}

func TestSaveLoadBinaryEmpty(t *testing.T) {
	path := filepath.Join(t.TempDir(), "empty.bin")
	require.NoError(t, SaveBinary(path, nil))
	loaded, err := LoadBinary(path)
	require.NoError(t, err)
	assert.Empty(t, loaded)
}

func TestSaveBinaryRejectsInvalidItems(t *testing.T) {
	path := filepath.Join(t.TempDir(), "invalid.bin")
	err := SaveBinary(path, []Container{NewArray("")})
	assert.Error(t, err)

	err = SaveBinary(path, []Container{NewArray("same"), NewArray("same")})
	assert.Error(t, err)

	err = SaveBinary(path, []Container{nil})
	assert.Error(t, err)
}

func TestLoadBinaryRejectsBadFormat(t *testing.T) {
	path := filepath.Join(t.TempDir(), "bad.bin")
	require.NoError(t, os.WriteFile(path, []byte("BAD!"), 0o644))
	_, err := LoadBinary(path)
	assert.Error(t, err)

	badVersion := buildBinaryTestFile()
	binary.LittleEndian.PutUint32(badVersion[len(binaryMagic):], binaryVersion+1)
	require.NoError(t, os.WriteFile(path, badVersion, 0o644))
	_, err = LoadBinary(path)
	assert.Error(t, err)

	truncated := buildBinaryTestFile(binaryTestRecord{
		typeName: TypeArray,
		name:     "arr",
		payload:  mustBinaryPayload(t, NewArray("arr")),
	})
	require.NoError(t, os.WriteFile(path, truncated[:len(truncated)-1], 0o644))
	_, err = LoadBinary(path)
	assert.Error(t, err)
}

func TestLoadBinaryRejectsBadRecords(t *testing.T) {
	path := filepath.Join(t.TempDir(), "bad-records.bin")
	cases := []struct {
		name    string
		records []binaryTestRecord
	}{
		{
			name:    "empty name",
			records: []binaryTestRecord{{typeName: TypeArray, name: "", payload: mustBinaryPayload(t, NewArray("x"))}},
		},
		{
			name: "duplicate name",
			records: []binaryTestRecord{
				{typeName: TypeArray, name: "same", payload: mustBinaryPayload(t, NewArray("a"))},
				{typeName: TypeArray, name: "same", payload: mustBinaryPayload(t, NewArray("b"))},
			},
		},
		{
			name:    "unknown type",
			records: []binaryTestRecord{{typeName: "unknown", name: "x", payload: []byte("xxxx")}},
		},
		{
			name:    "bad payload tag",
			records: []binaryTestRecord{{typeName: TypeArray, name: "arr", payload: []byte("BAD1")}},
		},
		{
			name: "bad tree marker",
			records: []binaryTestRecord{{
				typeName: TypeTree,
				name:     "tree",
				payload:  []byte("TRE1\x01\x00\x00\x00\x02"),
			}},
		},
	}
	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			require.NoError(t, os.WriteFile(path, buildBinaryTestFile(tc.records...), 0o644))
			_, err := LoadBinary(path)
			assert.Error(t, err)
		})
	}
}

func mustBinaryPayload(t *testing.T, item Container) []byte {
	t.Helper()
	payload, err := encodeBinaryPayload(item)
	require.NoError(t, err)
	return payload
}
