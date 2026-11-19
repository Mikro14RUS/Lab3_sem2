package containers

import (
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"
	"os"
	"sort"
)

const (
	binaryMagic        = "DS3BIN"
	binaryVersion      = 1
	maxBinaryItems     = 1000000
	maxBinaryString    = 16 * 1024 * 1024
	maxBinaryPayload   = 64 * 1024 * 1024
	minBinaryTreeValue = -1 << 31
	maxBinaryTreeValue = 1<<31 - 1
)

// SaveBinary записывает контейнеры в бинарный файл с сигнатурой DS3BIN.
// Имена сортируются, чтобы одинаковый набор данных давал одинаковый файл.
func SaveBinary(path string, items []Container) error {
	if len(items) > maxBinaryItems {
		return fmt.Errorf("слишком много структур: %d", len(items))
	}
	for _, item := range items {
		if item == nil {
			return errors.New("структура не может быть nil")
		}
	}
	ordered := append([]Container(nil), items...)
	sort.Slice(ordered, func(i, j int) bool { return ordered[i].Name() < ordered[j].Name() })

	var out bytes.Buffer
	out.WriteString(binaryMagic)
	writeBinaryUint32(&out, binaryVersion)
	writeBinaryUint32(&out, uint32(len(ordered)))

	names := make(map[string]bool, len(ordered))
	for _, item := range ordered {
		if item.Name() == "" {
			return errors.New("имя структуры не может быть пустым")
		}
		if names[item.Name()] {
			return fmt.Errorf("повторяющееся имя %q", item.Name())
		}
		names[item.Name()] = true

		payload, err := encodeBinaryPayload(item)
		if err != nil {
			return fmt.Errorf("сериализация %q: %w", item.Name(), err)
		}
		if len(payload) > maxBinaryPayload {
			return fmt.Errorf("слишком большой payload для %q", item.Name())
		}
		if err := writeBinaryString(&out, item.Type()); err != nil {
			return fmt.Errorf("сериализация %q: %w", item.Name(), err)
		}
		if err := writeBinaryString(&out, item.Name()); err != nil {
			return fmt.Errorf("сериализация %q: %w", item.Name(), err)
		}
		writeBinaryUint32(&out, uint32(len(payload)))
		out.Write(payload)
	}
	return os.WriteFile(path, out.Bytes(), 0o644)
}

// LoadBinary читает бинарный файл DS3BIN и восстанавливает контейнеры
// по полю type. При ошибке возвращается error без частичного результата.
func LoadBinary(path string) ([]Container, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("не удалось прочитать файл: %w", err)
	}
	reader := binaryReader{data: data}
	if magic := reader.readBytes(len(binaryMagic)); string(magic) != binaryMagic {
		return nil, fmt.Errorf("неверная сигнатура формата")
	}
	if version := reader.readUint32(); version != binaryVersion {
		return nil, fmt.Errorf("неподдерживаемая версия формата %d", version)
	}
	count := reader.readUint32()
	if reader.err != nil {
		return nil, fmt.Errorf("неверный формат бинарного файла: %w", reader.err)
	}
	if count > maxBinaryItems {
		return nil, fmt.Errorf("слишком много структур в файле: %d", count)
	}

	result := make([]Container, 0, count)
	names := make(map[string]bool, count)
	for i := uint32(0); i < count; i++ {
		typeName := reader.readString()
		name := reader.readString()
		payloadLength := reader.readUint32()
		if reader.err != nil {
			return nil, fmt.Errorf("неверный формат бинарного файла: %w", reader.err)
		}
		if payloadLength > maxBinaryPayload {
			return nil, fmt.Errorf("слишком большой payload у записи %d", i)
		}
		payload := reader.readBytes(int(payloadLength))
		if reader.err != nil {
			return nil, fmt.Errorf("неверный формат бинарного файла: %w", reader.err)
		}
		if name == "" {
			return nil, errors.New("пустое имя структуры в файле")
		}
		if names[name] {
			return nil, fmt.Errorf("повторяющееся имя %q", name)
		}
		names[name] = true

		item, err := NewByType(typeName, name)
		if err != nil {
			return nil, err
		}
		if err := decodeBinaryPayload(item, payload); err != nil {
			return nil, fmt.Errorf("повреждённые данные %q: %w", name, err)
		}
		result = append(result, item)
	}
	if reader.err != nil {
		return nil, fmt.Errorf("неверный формат бинарного файла: %w", reader.err)
	}
	if !reader.eof() {
		return nil, errors.New("лишние данные после последней структуры")
	}
	return result, nil
}

func encodeBinaryPayload(item Container) ([]byte, error) {
	switch item.Type() {
	case TypeArray:
		value, ok := item.(*Array)
		if !ok {
			return nil, errors.New("неверный тип контейнера array")
		}
		return encodeBinarySequence("ARR1", value.data)
	case TypeList:
		value, ok := item.(*SinglyLinkedList)
		if !ok {
			return nil, errors.New("неверный тип контейнера list")
		}
		return encodeBinarySequence("LST1", value.Values())
	case TypeDList:
		value, ok := item.(*DoublyLinkedList)
		if !ok {
			return nil, errors.New("неверный тип контейнера dlist")
		}
		return encodeBinarySequence("DLST", value.Values())
	case TypeStack:
		value, ok := item.(*Stack)
		if !ok {
			return nil, errors.New("неверный тип контейнера stack")
		}
		return encodeBinarySequence("STK1", value.data)
	case TypeQueue:
		value, ok := item.(*Queue)
		if !ok {
			return nil, errors.New("неверный тип контейнера queue")
		}
		return encodeBinarySequence("QUE1", value.Values())
	case TypeHash:
		value, ok := item.(*HashTable)
		if !ok {
			return nil, errors.New("неверный тип контейнера hash")
		}
		return encodeBinaryHash(value)
	case TypeTree:
		value, ok := item.(*FullBinaryTree)
		if !ok {
			return nil, errors.New("неверный тип контейнера tree")
		}
		return encodeBinaryTree(value)
	default:
		return nil, fmt.Errorf("неизвестный тип структуры %q", item.Type())
	}
}

func decodeBinaryPayload(item Container, payload []byte) error {
	switch item.Type() {
	case TypeArray:
		value, ok := item.(*Array)
		if !ok {
			return errors.New("неверный тип контейнера array")
		}
		values, err := decodeBinarySequence(payload, "ARR1")
		if err != nil {
			return err
		}
		value.data = values
		return nil
	case TypeList:
		value, ok := item.(*SinglyLinkedList)
		if !ok {
			return errors.New("неверный тип контейнера list")
		}
		values, err := decodeBinarySequence(payload, "LST1")
		if err != nil {
			return err
		}
		value.Clear()
		for _, current := range values {
			value.PushBack(current)
		}
		return nil
	case TypeDList:
		value, ok := item.(*DoublyLinkedList)
		if !ok {
			return errors.New("неверный тип контейнера dlist")
		}
		values, err := decodeBinarySequence(payload, "DLST")
		if err != nil {
			return err
		}
		value.Clear()
		for _, current := range values {
			value.PushBack(current)
		}
		return nil
	case TypeStack:
		value, ok := item.(*Stack)
		if !ok {
			return errors.New("неверный тип контейнера stack")
		}
		values, err := decodeBinarySequence(payload, "STK1")
		if err != nil {
			return err
		}
		value.data = values
		return nil
	case TypeQueue:
		value, ok := item.(*Queue)
		if !ok {
			return errors.New("неверный тип контейнера queue")
		}
		values, err := decodeBinarySequence(payload, "QUE1")
		if err != nil {
			return err
		}
		value.Clear()
		value.data = append(value.data, values...)
		return nil
	case TypeHash:
		value, ok := item.(*HashTable)
		if !ok {
			return errors.New("неверный тип контейнера hash")
		}
		return decodeBinaryHash(value, payload)
	case TypeTree:
		value, ok := item.(*FullBinaryTree)
		if !ok {
			return errors.New("неверный тип контейнера tree")
		}
		return decodeBinaryTree(value, payload)
	default:
		return fmt.Errorf("неизвестный тип структуры %q", item.Type())
	}
}

func encodeBinarySequence(tag string, values []string) ([]byte, error) {
	if len(values) > maxBinaryItems {
		return nil, fmt.Errorf("слишком много элементов: %d", len(values))
	}
	var out bytes.Buffer
	out.WriteString(tag)
	writeBinaryUint32(&out, uint32(len(values)))
	for _, value := range values {
		if err := writeBinaryString(&out, value); err != nil {
			return nil, err
		}
	}
	return out.Bytes(), nil
}

func decodeBinarySequence(payload []byte, tag string) ([]string, error) {
	reader := binaryReader{data: payload}
	if !reader.readTag(tag) {
		return nil, fmt.Errorf("неверный тег формата, ожидался %s", tag)
	}
	count := reader.readUint32()
	if reader.err != nil {
		return nil, reader.err
	}
	if count > maxBinaryItems {
		return nil, fmt.Errorf("слишком много элементов: %d", count)
	}
	values := make([]string, 0, count)
	for i := uint32(0); i < count; i++ {
		values = append(values, reader.readString())
		if reader.err != nil {
			return nil, reader.err
		}
	}
	if !reader.eof() {
		return nil, errors.New("лишние данные после последовательности")
	}
	return values, nil
}

func encodeBinaryHash(table *HashTable) ([]byte, error) {
	entries := table.Entries()
	if len(entries) > maxBinaryItems {
		return nil, fmt.Errorf("слишком много пар: %d", len(entries))
	}
	var out bytes.Buffer
	out.WriteString("HSH1")
	writeBinaryUint32(&out, uint32(len(entries)))
	for _, entry := range entries {
		if err := writeBinaryString(&out, entry.Key); err != nil {
			return nil, err
		}
		if err := writeBinaryString(&out, entry.Value); err != nil {
			return nil, err
		}
	}
	return out.Bytes(), nil
}

func decodeBinaryHash(table *HashTable, payload []byte) error {
	reader := binaryReader{data: payload}
	if !reader.readTag("HSH1") {
		return errors.New("неверный тег формата, ожидался HSH1")
	}
	count := reader.readUint32()
	if reader.err != nil {
		return reader.err
	}
	if count > maxBinaryItems {
		return fmt.Errorf("слишком много пар: %d", count)
	}
	table.Clear()
	for i := uint32(0); i < count; i++ {
		key := reader.readString()
		value := reader.readString()
		if reader.err != nil {
			return reader.err
		}
		table.Put(key, value)
	}
	if !reader.eof() {
		return errors.New("лишние данные после хеш-таблицы")
	}
	return nil
}

func encodeBinaryTree(tree *FullBinaryTree) ([]byte, error) {
	tokens := tree.PreorderWithNulls()
	if len(tokens) > maxBinaryItems {
		return nil, fmt.Errorf("слишком много токенов дерева: %d", len(tokens))
	}
	var out bytes.Buffer
	out.WriteString("TRE1")
	writeBinaryUint32(&out, uint32(len(tokens)))
	for _, token := range tokens {
		if token == nil {
			out.WriteByte(0)
			continue
		}
		if int64(*token) < minBinaryTreeValue || int64(*token) > maxBinaryTreeValue {
			return nil, fmt.Errorf("значение дерева вне диапазона int32: %d", *token)
		}
		out.WriteByte(1)
		writeBinaryUint32(&out, uint32(int32(*token)))
	}
	return out.Bytes(), nil
}

func decodeBinaryTree(tree *FullBinaryTree, payload []byte) error {
	reader := binaryReader{data: payload}
	if !reader.readTag("TRE1") {
		return errors.New("неверный тег формата, ожидался TRE1")
	}
	count := reader.readUint32()
	if reader.err != nil {
		return reader.err
	}
	if count > maxBinaryItems {
		return fmt.Errorf("слишком много токенов дерева: %d", count)
	}
	tokens := make([]*int, 0, count)
	for i := uint32(0); i < count; i++ {
		marker := reader.readByte()
		if reader.err != nil {
			return reader.err
		}
		switch marker {
		case 0:
			tokens = append(tokens, nil)
		case 1:
			raw := reader.readUint32()
			if reader.err != nil {
				return reader.err
			}
			value := int(int32(raw))
			tokens = append(tokens, &value)
		default:
			return fmt.Errorf("неверный маркер дерева: %d", marker)
		}
	}
	if !reader.eof() {
		return errors.New("лишние данные после дерева")
	}
	if !tree.LoadPreorderWithNulls(tokens) {
		return errors.New("некорректный preorder-формат дерева")
	}
	return nil
}

func writeBinaryUint32(out *bytes.Buffer, value uint32) {
	var data [4]byte
	binary.LittleEndian.PutUint32(data[:], value)
	out.Write(data[:])
}

func writeBinaryString(out *bytes.Buffer, value string) error {
	if len(value) > maxBinaryString {
		return fmt.Errorf("слишком длинная строка: %d", len(value))
	}
	writeBinaryUint32(out, uint32(len(value)))
	out.WriteString(value)
	return nil
}

type binaryReader struct {
	data   []byte
	offset int
	err    error
}

func (r *binaryReader) readBytes(count int) []byte {
	if r.err != nil {
		return nil
	}
	if count < 0 || r.offset+count > len(r.data) {
		r.err = errors.New("неожиданный конец файла")
		return nil
	}
	value := r.data[r.offset : r.offset+count]
	r.offset += count
	return value
}

func (r *binaryReader) readByte() byte {
	value := r.readBytes(1)
	if r.err != nil {
		return 0
	}
	return value[0]
}

func (r *binaryReader) readUint32() uint32 {
	value := r.readBytes(4)
	if r.err != nil {
		return 0
	}
	return binary.LittleEndian.Uint32(value)
}

func (r *binaryReader) readString() string {
	length := r.readUint32()
	if r.err != nil {
		return ""
	}
	if length > maxBinaryString {
		r.err = fmt.Errorf("слишком длинная строка: %d", length)
		return ""
	}
	return string(r.readBytes(int(length)))
}

func (r *binaryReader) readTag(tag string) bool {
	value := r.readBytes(len(tag))
	return r.err == nil && string(value) == tag
}

func (r *binaryReader) eof() bool {
	return r.err == nil && r.offset == len(r.data)
}
