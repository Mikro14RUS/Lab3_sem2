package containers

import (
	"encoding/json"
	"fmt"
	"strings"
)

type singlyNode struct {
	value string
	next  *singlyNode
}

// SinglyLinkedList — односвязный список с указателями head и tail.
type SinglyLinkedList struct {
	name string
	head *singlyNode
	tail *singlyNode
	size int
}

func NewSinglyLinkedList(name string) *SinglyLinkedList {
	return &SinglyLinkedList{name: name}
}

func (l *SinglyLinkedList) Type() string { return TypeList }
func (l *SinglyLinkedList) Name() string { return l.name }
func (l *SinglyLinkedList) Len() int     { return l.size }

func (l *SinglyLinkedList) Clear() {
	l.head = nil
	l.tail = nil
	l.size = 0
}

func (l *SinglyLinkedList) PushFront(value string) {
	node := &singlyNode{value: value, next: l.head}
	l.head = node
	if l.tail == nil {
		l.tail = node
	}
	l.size++
}

func (l *SinglyLinkedList) PushBack(value string) {
	node := &singlyNode{value: value}
	if l.tail == nil {
		l.head = node
	} else {
		l.tail.next = node
	}
	l.tail = node
	l.size++
}

func (l *SinglyLinkedList) InsertBefore(target, value string) bool {
	if l.head == nil {
		return false
	}
	if l.head.value == target {
		l.PushFront(value)
		return true
	}
	previous := l.head
	for previous.next != nil && previous.next.value != target {
		previous = previous.next
	}
	if previous.next == nil {
		return false
	}
	previous.next = &singlyNode{value: value, next: previous.next}
	l.size++
	return true
}

func (l *SinglyLinkedList) InsertAfter(target, value string) bool {
	node := l.findNode(target)
	if node == nil {
		return false
	}
	node.next = &singlyNode{value: value, next: node.next}
	if node == l.tail {
		l.tail = node.next
	}
	l.size++
	return true
}

func (l *SinglyLinkedList) PopFront() (string, bool) {
	if l.head == nil {
		return "", false
	}
	node := l.head
	l.head = node.next
	if l.head == nil {
		l.tail = nil
	}
	l.size--
	return node.value, true
}

func (l *SinglyLinkedList) PopBack() (string, bool) {
	if l.head == nil {
		return "", false
	}
	if l.head == l.tail {
		value := l.head.value
		l.Clear()
		return value, true
	}
	previous := l.head
	for previous.next != l.tail {
		previous = previous.next
	}
	value := l.tail.value
	previous.next = nil
	l.tail = previous
	l.size--
	return value, true
}

func (l *SinglyLinkedList) DeleteBefore(target string) (string, bool) {
	if l.head == nil || l.head.value == target {
		return "", false
	}
	previous := l.head
	for previous.next != nil && previous.next.value != target {
		previous = previous.next
	}
	if previous.next == nil {
		return "", false
	}
	if previous == l.head {
		return l.PopFront()
	}
	beforePrevious := l.head
	for beforePrevious.next != previous {
		beforePrevious = beforePrevious.next
	}
	value := previous.value
	beforePrevious.next = previous.next
	l.size--
	return value, true
}

func (l *SinglyLinkedList) DeleteAfter(target string) (string, bool) {
	node := l.findNode(target)
	if node == nil || node.next == nil {
		return "", false
	}
	removed := node.next
	node.next = removed.next
	if removed == l.tail {
		l.tail = node
	}
	l.size--
	return removed.value, true
}

func (l *SinglyLinkedList) DeleteValue(value string) (string, bool) {
	if l.head == nil {
		return "", false
	}
	if l.head.value == value {
		return l.PopFront()
	}
	previous := l.head
	for previous.next != nil && previous.next.value != value {
		previous = previous.next
	}
	if previous.next == nil {
		return "", false
	}
	removed := previous.next
	previous.next = removed.next
	if removed == l.tail {
		l.tail = previous
	}
	l.size--
	return removed.value, true
}

func (l *SinglyLinkedList) Find(value string) int {
	index := 0
	for node := l.head; node != nil; node = node.next {
		if node.value == value {
			return index
		}
		index++
	}
	return -1
}

func (l *SinglyLinkedList) GetAt(index int) (string, bool) {
	node := l.nodeAt(index)
	if node == nil {
		return "", false
	}
	return node.value, true
}

func (l *SinglyLinkedList) SetAt(index int, value string) bool {
	node := l.nodeAt(index)
	if node == nil {
		return false
	}
	node.value = value
	return true
}

func (l *SinglyLinkedList) Values() []string {
	result := make([]string, 0, l.size)
	for node := l.head; node != nil; node = node.next {
		result = append(result, node.value)
	}
	return result
}

func (l *SinglyLinkedList) String() string {
	if l.size == 0 {
		return "[]"
	}
	return "[" + strings.Join(l.Values(), " -> ") + "]"
}

func (l *SinglyLinkedList) nodeAt(index int) *singlyNode {
	if index < 0 || index >= l.size {
		return nil
	}
	node := l.head
	for i := 0; i < index; i++ {
		node = node.next
	}
	return node
}

func (l *SinglyLinkedList) findNode(value string) *singlyNode {
	for node := l.head; node != nil; node = node.next {
		if node.value == value {
			return node
		}
	}
	return nil
}

func (l *SinglyLinkedList) MarshalJSON() ([]byte, error) {
	return json.Marshal(l.Values())
}

func (l *SinglyLinkedList) UnmarshalJSON(data []byte) error {
	var values []string
	if err := json.Unmarshal(data, &values); err != nil {
		return fmt.Errorf("list: %w", err)
	}
	l.Clear()
	for _, value := range values {
		l.PushBack(value)
	}
	return nil
}
