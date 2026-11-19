package containers

import (
	"encoding/json"
	"fmt"
	"strings"
)

// Queue — очередь FIFO. Используется кольцевой буфер на срезе:
// front указывает на первый элемент, данные не сдвигаются при pop.
type Queue struct {
	name  string
	data  []string
	front int
}

func NewQueue(name string) *Queue {
	return &Queue{name: name, data: make([]string, 0)}
}

func (q *Queue) Type() string { return TypeQueue }
func (q *Queue) Name() string { return q.name }
func (q *Queue) Len() int     { return len(q.data) - q.front }

func (q *Queue) Clear() {
	q.data = q.data[:0]
	q.front = 0
}

func (q *Queue) Push(value string) { q.data = append(q.data, value) }

func (q *Queue) Pop() (string, bool) {
	if q.front >= len(q.data) {
		return "", false
	}
	value := q.data[q.front]
	q.front++
	if q.front > 0 && q.front*2 >= len(q.data) {
		q.data = append([]string(nil), q.data[q.front:]...)
		q.front = 0
	}
	return value, true
}

func (q *Queue) Front() (string, bool) {
	if q.front >= len(q.data) {
		return "", false
	}
	return q.data[q.front], true
}

func (q *Queue) Values() []string {
	result := make([]string, 0, q.Len())
	result = append(result, q.data[q.front:]...)
	return result
}

func (q *Queue) String() string {
	values := q.Values()
	if len(values) == 0 {
		return "начало -> <- конец"
	}
	return "начало -> " + strings.Join(values, " ") + " <- конец"
}

func (q *Queue) MarshalJSON() ([]byte, error) {
	return json.Marshal(q.Values())
}

func (q *Queue) UnmarshalJSON(data []byte) error {
	var values []string
	if err := json.Unmarshal(data, &values); err != nil {
		return fmt.Errorf("queue: %w", err)
	}
	q.Clear()
	q.data = append(q.data, values...)
	return nil
}
