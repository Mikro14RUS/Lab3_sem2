package containers

import (
	"encoding/json"
	"fmt"
	"strings"
)

// Array — динамический массив строк. В Go срез уже является
// динамическим массивом, поэтому структура хранит его внутри
// и предоставляет предметный интерфейс ЛР1.
type Array struct {
	name string
	data []string
}

func NewArray(name string) *Array {
	return &Array{name: name, data: make([]string, 0)}
}

func (a *Array) Type() string { return TypeArray }
func (a *Array) Name() string { return a.name }
func (a *Array) Len() int     { return len(a.data) }
func (a *Array) Clear()       { a.data = a.data[:0] }

func (a *Array) PushBack(value string) { a.data = append(a.data, value) }

func (a *Array) PushFront(value string) {
	a.data = append(a.data, "")
	copy(a.data[1:], a.data[:len(a.data)-1])
	a.data[0] = value
}

func (a *Array) InsertAt(index int, value string) bool {
	if index < 0 || index > len(a.data) {
		return false
	}
	a.data = append(a.data, "")
	copy(a.data[index+1:], a.data[index:])
	a.data[index] = value
	return true
}

func (a *Array) PopBack() (string, bool) {
	if len(a.data) == 0 {
		return "", false
	}
	value := a.data[len(a.data)-1]
	a.data = a.data[:len(a.data)-1]
	return value, true
}

func (a *Array) PopFront() (string, bool) {
	if len(a.data) == 0 {
		return "", false
	}
	value := a.data[0]
	a.data = a.data[1:]
	return value, true
}

func (a *Array) RemoveAt(index int) (string, bool) {
	if index < 0 || index >= len(a.data) {
		return "", false
	}
	value := a.data[index]
	copy(a.data[index:], a.data[index+1:])
	a.data = a.data[:len(a.data)-1]
	return value, true
}

func (a *Array) GetAt(index int) (string, bool) {
	if index < 0 || index >= len(a.data) {
		return "", false
	}
	return a.data[index], true
}

func (a *Array) SetAt(index int, value string) bool {
	if index < 0 || index >= len(a.data) {
		return false
	}
	a.data[index] = value
	return true
}

func (a *Array) Values() []string {
	result := make([]string, len(a.data))
	copy(result, a.data)
	return result
}

func (a *Array) String() string {
	if len(a.data) == 0 {
		return "[]"
	}
	return "[" + strings.Join(a.data, ", ") + "]"
}

func (a *Array) MarshalJSON() ([]byte, error) {
	if a.data == nil {
		return []byte("[]"), nil
	}
	return json.Marshal(a.data)
}

func (a *Array) UnmarshalJSON(data []byte) error {
	var values []string
	if err := json.Unmarshal(data, &values); err != nil {
		return fmt.Errorf("array: %w", err)
	}
	a.data = values
	if a.data == nil {
		a.data = make([]string, 0)
	}
	return nil
}
