package containers

import (
	"bytes"
	"encoding/binary"
	"errors"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// fakeBinaryContainer lets the tests exercise type-dispatch errors without
// creating an invalid real container.
type fakeBinaryContainer struct {
	typ  string
	name string
}

func (f fakeBinaryContainer) Type() string                 { return f.typ }
func (f fakeBinaryContainer) Name() string                 { return f.name }
func (f fakeBinaryContainer) Len() int                     { return 0 }
func (f fakeBinaryContainer) Clear()                       {}
func (f fakeBinaryContainer) String() string               { return "" }
func (f fakeBinaryContainer) MarshalJSON() ([]byte, error) { return []byte("{}"), nil }
func (f fakeBinaryContainer) UnmarshalJSON([]byte) error   { return nil }

func binaryUint32Bytes(value uint32) []byte {
	var data [4]byte
	binary.LittleEndian.PutUint32(data[:], value)
	return data[:]
}

func TestSaveBinaryRejectsTooManyItems(t *testing.T) {
	path := filepath.Join(t.TempDir(), "too-many.bin")
	err := SaveBinary(path, make([]Container, maxBinaryItems+1))
	require.Error(t, err)
	assert.Contains(t, err.Error(), "слишком много структур")
}

func TestSaveBinaryRejectsTooLongName(t *testing.T) {
	path := filepath.Join(t.TempDir(), "long-name.bin")
	name := strings.Repeat("n", maxBinaryString+1)
	err := SaveBinary(path, []Container{NewArray(name)})
	require.Error(t, err)
	assert.Contains(t, err.Error(), "слишком длинная строка")
}

func TestBinaryPayloadRejectsWrongContainerTypes(t *testing.T) {
	knownTypes := []string{
		TypeArray,
		TypeList,
		TypeDList,
		TypeStack,
		TypeQueue,
		TypeHash,
		TypeTree,
	}
	for _, typ := range knownTypes {
		t.Run("encode_"+typ, func(t *testing.T) {
			path := filepath.Join(t.TempDir(), "wrong-type.bin")
			err := SaveBinary(path, []Container{fakeBinaryContainer{typ: typ, name: "fake"}})
			require.Error(t, err)
			assert.Contains(t, err.Error(), "неверный тип контейнера")
		})
		t.Run("decode_"+typ, func(t *testing.T) {
			err := decodeBinaryPayload(fakeBinaryContainer{typ: typ, name: "fake"}, nil)
			require.Error(t, err)
			assert.Contains(t, err.Error(), "неверный тип контейнера")
		})
	}

	t.Run("unknown_encode", func(t *testing.T) {
		path := filepath.Join(t.TempDir(), "unknown-type.bin")
		err := SaveBinary(path, []Container{fakeBinaryContainer{typ: "unknown", name: "fake"}})
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неизвестный тип структуры")
	})
	t.Run("unknown_decode", func(t *testing.T) {
		err := decodeBinaryPayload(fakeBinaryContainer{typ: "unknown", name: "fake"}, nil)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неизвестный тип структуры")
	})
}

func TestLoadBinaryRejectsUnreadablePath(t *testing.T) {
	path := filepath.Join(t.TempDir(), "missing.bin")
	_, err := LoadBinary(path)
	require.Error(t, err)
	assert.Contains(t, err.Error(), "не удалось прочитать файл")
}

func TestLoadBinaryRejectsOversizedHeaderValues(t *testing.T) {
	t.Run("too many records", func(t *testing.T) {
		path := filepath.Join(t.TempDir(), "too-many.bin")
		var out bytes.Buffer
		out.WriteString(binaryMagic)
		writeBinaryUint32(&out, binaryVersion)
		writeBinaryUint32(&out, maxBinaryItems+1)
		require.NoError(t, os.WriteFile(path, out.Bytes(), 0o644))

		_, err := LoadBinary(path)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком много структур в файле")
	})

	t.Run("too large payload length", func(t *testing.T) {
		path := filepath.Join(t.TempDir(), "large-payload.bin")
		var out bytes.Buffer
		out.WriteString(binaryMagic)
		writeBinaryUint32(&out, binaryVersion)
		writeBinaryUint32(&out, 1)
		require.NoError(t, writeBinaryString(&out, TypeArray))
		require.NoError(t, writeBinaryString(&out, "arr"))
		writeBinaryUint32(&out, maxBinaryPayload+1)
		require.NoError(t, os.WriteFile(path, out.Bytes(), 0o644))

		_, err := LoadBinary(path)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком большой payload")
	})
}

func TestLoadBinaryRejectsTrailingBytes(t *testing.T) {
	path := filepath.Join(t.TempDir(), "trailing.bin")
	data := buildBinaryTestFile()
	data = append(data, 0x7f)
	require.NoError(t, os.WriteFile(path, data, 0o644))

	_, err := LoadBinary(path)
	require.Error(t, err)
	assert.Contains(t, err.Error(), "лишние данные")
}

func TestBinarySequenceValidation(t *testing.T) {
	t.Run("encode too many values", func(t *testing.T) {
		_, err := encodeBinarySequence("ARR1", make([]string, maxBinaryItems+1))
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком много элементов")
	})

	t.Run("encode too long value", func(t *testing.T) {
		values := []string{strings.Repeat("x", maxBinaryString+1)}
		_, err := encodeBinarySequence("ARR1", values)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком длинная строка")
	})

	t.Run("decode wrong tag", func(t *testing.T) {
		_, err := decodeBinarySequence([]byte("BAD1"), "ARR1")
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неверный тег")
	})

	t.Run("decode too many values", func(t *testing.T) {
		payload := append([]byte("ARR1"), binaryUint32Bytes(maxBinaryItems+1)...)
		_, err := decodeBinarySequence(payload, "ARR1")
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком много элементов")
	})

	t.Run("decode truncated string", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("ARR1")
		writeBinaryUint32(&out, 1)
		writeBinaryUint32(&out, 5)
		_, err := decodeBinarySequence(out.Bytes(), "ARR1")
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неожиданный конец файла")
	})

	t.Run("decode trailing bytes", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("ARR1")
		writeBinaryUint32(&out, 0)
		out.WriteByte(1)
		_, err := decodeBinarySequence(out.Bytes(), "ARR1")
		require.Error(t, err)
		assert.Contains(t, err.Error(), "лишние данные")
	})
}

func TestBinaryHashValidation(t *testing.T) {
	t.Run("encode too long key", func(t *testing.T) {
		table := NewHashTable("hash", 4)
		table.Put(strings.Repeat("k", maxBinaryString+1), "value")
		_, err := encodeBinaryHash(table)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком длинная строка")
	})

	t.Run("encode too long value", func(t *testing.T) {
		table := NewHashTable("hash", 4)
		table.Put("key", strings.Repeat("v", maxBinaryString+1))
		_, err := encodeBinaryHash(table)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком длинная строка")
	})

	t.Run("decode wrong tag", func(t *testing.T) {
		err := decodeBinaryHash(NewHashTable("hash", 4), []byte("BAD1"))
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неверный тег")
	})

	t.Run("decode too many pairs", func(t *testing.T) {
		payload := append([]byte("HSH1"), binaryUint32Bytes(maxBinaryItems+1)...)
		err := decodeBinaryHash(NewHashTable("hash", 4), payload)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком много пар")
	})

	t.Run("decode truncated pair", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("HSH1")
		writeBinaryUint32(&out, 1)
		require.NoError(t, writeBinaryString(&out, "key"))
		err := decodeBinaryHash(NewHashTable("hash", 4), out.Bytes())
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неожиданный конец файла")
	})

	t.Run("decode trailing bytes", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("HSH1")
		writeBinaryUint32(&out, 0)
		out.WriteByte(1)
		err := decodeBinaryHash(NewHashTable("hash", 4), out.Bytes())
		require.Error(t, err)
		assert.Contains(t, err.Error(), "лишние данные")
	})
}

func TestBinaryTreeValidation(t *testing.T) {
	t.Run("encode value outside int32", func(t *testing.T) {
		tree := NewFullBinaryTree("tree")
		require.True(t, tree.Insert(maxBinaryTreeValue+1))
		_, err := encodeBinaryTree(tree)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "вне диапазона int32")
	})

	t.Run("decode wrong tag", func(t *testing.T) {
		err := decodeBinaryTree(NewFullBinaryTree("tree"), []byte("BAD1"))
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неверный тег")
	})

	t.Run("decode too many tokens", func(t *testing.T) {
		payload := append([]byte("TRE1"), binaryUint32Bytes(maxBinaryItems+1)...)
		err := decodeBinaryTree(NewFullBinaryTree("tree"), payload)
		require.Error(t, err)
		assert.Contains(t, err.Error(), "слишком много токенов")
	})

	t.Run("decode truncated marker", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("TRE1")
		writeBinaryUint32(&out, 1)
		err := decodeBinaryTree(NewFullBinaryTree("tree"), out.Bytes())
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неожиданный конец файла")
	})

	t.Run("decode truncated value", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("TRE1")
		writeBinaryUint32(&out, 1)
		out.WriteByte(1)
		err := decodeBinaryTree(NewFullBinaryTree("tree"), out.Bytes())
		require.Error(t, err)
		assert.Contains(t, err.Error(), "неожиданный конец файла")
	})

	t.Run("decode trailing bytes", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("TRE1")
		writeBinaryUint32(&out, 0)
		out.WriteByte(1)
		err := decodeBinaryTree(NewFullBinaryTree("tree"), out.Bytes())
		require.Error(t, err)
		assert.Contains(t, err.Error(), "лишние данные")
	})

	t.Run("decode invalid preorder", func(t *testing.T) {
		var out bytes.Buffer
		out.WriteString("TRE1")
		writeBinaryUint32(&out, 2)
		out.WriteByte(0)
		out.WriteByte(0)
		err := decodeBinaryTree(NewFullBinaryTree("tree"), out.Bytes())
		require.Error(t, err)
		assert.Contains(t, err.Error(), "некорректный preorder")
	})
}

func TestBinaryReaderEdgeCases(t *testing.T) {
	t.Run("previous error", func(t *testing.T) {
		reader := binaryReader{err: errors.New("broken")}
		assert.Nil(t, reader.readBytes(1))
		assert.Equal(t, byte(0), reader.readByte())
		assert.Equal(t, uint32(0), reader.readUint32())
		assert.Equal(t, "", reader.readString())
		assert.False(t, reader.readTag("ARR1"))
		assert.False(t, reader.eof())
	})

	t.Run("truncated primitive values", func(t *testing.T) {
		reader := binaryReader{data: []byte{1}}
		assert.Nil(t, reader.readBytes(2))
		assert.Equal(t, byte(0), reader.readByte())
		assert.Equal(t, uint32(0), reader.readUint32())
		assert.Equal(t, "", reader.readString())
	})

	t.Run("negative read length", func(t *testing.T) {
		reader := binaryReader{data: []byte{1}}
		assert.Nil(t, reader.readBytes(-1))
		assert.Error(t, reader.err)
	})

	t.Run("too long string", func(t *testing.T) {
		reader := binaryReader{data: binaryUint32Bytes(maxBinaryString + 1)}
		assert.Equal(t, "", reader.readString())
		assert.Error(t, reader.err)
	})

	t.Run("tag mismatch and empty eof", func(t *testing.T) {
		reader := binaryReader{data: []byte("NO")}
		assert.False(t, reader.readTag("ARR1"))
		assert.False(t, reader.eof())
		assert.True(t, (&binaryReader{}).eof())
	})
}
