package containers

import (
	"encoding/json"
	"fmt"
	"strings"
)

// Stack — стек LIFO на срезе Go. Вершина хранится в конце среза.
type Stack struct {
	name string
	data []string
}

func NewStack(name string) *Stack {
	return &Stack{name: name, data: make([]string, 0)}
}

func (s *Stack) Type() string { return TypeStack }
func (s *Stack) Name() string { return s.name }
func (s *Stack) Len() int     { return len(s.data) }
func (s *Stack) Clear()       { s.data = s.data[:0] }

func (s *Stack) Push(value string) { s.data = append(s.data, value) }

func (s *Stack) Pop() (string, bool) {
	if len(s.data) == 0 {
		return "", false
	}
	value := s.data[len(s.data)-1]
	s.data = s.data[:len(s.data)-1]
	return value, true
}

func (s *Stack) Peek() (string, bool) {
	if len(s.data) == 0 {
		return "", false
	}
	return s.data[len(s.data)-1], true
}

func (s *Stack) Values() []string {
	result := make([]string, len(s.data))
	for i, value := range s.data {
		result[len(s.data)-1-i] = value
	}
	return result
}

func (s *Stack) String() string {
	if len(s.data) == 0 {
		return "[]"
	}
	values := s.Values()
	return "[" + strings.Join(values, ", ") + "]"
}

func (s *Stack) MarshalJSON() ([]byte, error) {
	if s.data == nil {
		return []byte("[]"), nil
	}
	return json.Marshal(s.data)
}

func (s *Stack) UnmarshalJSON(data []byte) error {
	var values []string
	if err := json.Unmarshal(data, &values); err != nil {
		return fmt.Errorf("stack: %w", err)
	}
	s.data = values
	if s.data == nil {
		s.data = make([]string, 0)
	}
	return nil
}
