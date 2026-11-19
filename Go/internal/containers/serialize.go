package containers

import (
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"sort"
)

const (
	formatMagic   = "DS3TXT"
	formatVersion = 1
)

type record struct {
	Type    string          `json:"type"`
	Name    string          `json:"name"`
	Payload json.RawMessage `json:"payload"`
}

type envelope struct {
	Magic   string   `json:"magic"`
	Version int      `json:"version"`
	Items   []record `json:"items"`
}

// NewByType создаёт контейнер указанного типа с заданным именем.
func NewByType(typeName, name string) (Container, error) {
	switch typeName {
	case TypeArray:
		return NewArray(name), nil
	case TypeList:
		return NewSinglyLinkedList(name), nil
	case TypeDList:
		return NewDoublyLinkedList(name), nil
	case TypeStack:
		return NewStack(name), nil
	case TypeQueue:
		return NewQueue(name), nil
	case TypeHash:
		return NewHashTable(name, 16), nil
	case TypeTree:
		return NewFullBinaryTree(name), nil
	default:
		return nil, fmt.Errorf("неизвестный тип структуры %q", typeName)
	}
}

// SaveText записывает контейнеры в текстовый JSON-файл. Имена сортируются,
// чтобы одинаковый набор данных давал одинаковый файл.
func SaveText(path string, items []Container) error {
	ordered := append([]Container(nil), items...)
	sort.Slice(ordered, func(i, j int) bool { return ordered[i].Name() < ordered[j].Name() })

	records := make([]record, 0, len(ordered))
	for _, item := range ordered {
		if item.Name() == "" {
			return errors.New("имя структуры не может быть пустым")
		}
		payload, err := json.Marshal(item)
		if err != nil {
			return fmt.Errorf("сериализация %q: %w", item.Name(), err)
		}
		records = append(records, record{Type: item.Type(), Name: item.Name(), Payload: payload})
	}
	data, err := json.MarshalIndent(envelope{
		Magic: formatMagic, Version: formatVersion, Items: records,
	}, "", "  ")
	if err != nil {
		return err
	}
	return os.WriteFile(path, append(data, '\n'), 0o644)
}

// LoadText читает JSON-файл и восстанавливает контейнеры по полю type.
func LoadText(path string) ([]Container, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("не удалось прочитать файл: %w", err)
	}
	var input envelope
	if err := json.Unmarshal(data, &input); err != nil {
		return nil, fmt.Errorf("неверный формат JSON: %w", err)
	}
	if input.Magic != formatMagic {
		return nil, fmt.Errorf("неверная сигнатура формата %q", input.Magic)
	}
	if input.Version != formatVersion {
		return nil, fmt.Errorf("неподдерживаемая версия формата %d", input.Version)
	}

	result := make([]Container, 0, len(input.Items))
	names := make(map[string]bool, len(input.Items))
	for _, item := range input.Items {
		if item.Name == "" {
			return nil, errors.New("пустое имя структуры в файле")
		}
		if names[item.Name] {
			return nil, fmt.Errorf("повторяющееся имя %q", item.Name)
		}
		names[item.Name] = true
		container, err := NewByType(item.Type, item.Name)
		if err != nil {
			return nil, err
		}
		if err := json.Unmarshal(item.Payload, container); err != nil {
			return nil, fmt.Errorf("повреждённые данные %q: %w", item.Name, err)
		}
		result = append(result, container)
	}
	return result, nil
}
