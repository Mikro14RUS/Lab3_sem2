package containers

import (
	"encoding/json"
	"fmt"
	"strings"
)

type doublyNode struct {
	value string
	prev  *doublyNode
	next  *doublyNode
}

// DoublyLinkedList — двусвязный список. Благодаря prev удаление
// хвоста и обратный обход выполняются за O(1).
type DoublyLinkedList struct {
	name string
	head *doublyNode
	tail *doublyNode
	size int
}

func NewDoublyLinkedList(name string) *DoublyLinkedList {
	return &DoublyLinkedList{name: name}
}

func (l *DoublyLinkedList) Type() string { return TypeDList }
func (l *DoublyLinkedList) Name() string { return l.name }
func (l *DoublyLinkedList) Len() int     { return l.size }

func (l *DoublyLinkedList) Clear() {
	l.head = nil
	l.tail = nil
	l.size = 0
}

func (l *DoublyLinkedList) PushFront(value string) {
	node := &doublyNode{value: value, next: l.head}
	if l.head != nil {
		l.head.prev = node
	} else {
		l.tail = node
	}
	l.head = node
	l.size++
}

func (l *DoublyLinkedList) PushBack(value string) {
	node := &doublyNode{value: value, prev: l.tail}
	if l.tail != nil {
		l.tail.next = node
	} else {
		l.head = node
	}
	l.tail = node
	l.size++
}

func (l *DoublyLinkedList) InsertBefore(target, value string) bool {
	node := l.findNode(target)
	if node == nil {
		return false
	}
	if node == l.head {
		l.PushFront(value)
		return true
	}
	inserted := &doublyNode{value: value, prev: node.prev, next: node}
	node.prev.next = inserted
	node.prev = inserted
	l.size++
	return true
}

func (l *DoublyLinkedList) InsertAfter(target, value string) bool {
	node := l.findNode(target)
	if node == nil {
		return false
	}
	if node == l.tail {
		l.PushBack(value)
		return true
	}
	inserted := &doublyNode{value: value, prev: node, next: node.next}
	node.next.prev = inserted
	node.next = inserted
	l.size++
	return true
}

func (l *DoublyLinkedList) PopFront() (string, bool) {
	if l.head == nil {
		return "", false
	}
	node := l.head
	l.head = node.next
	if l.head != nil {
		l.head.prev = nil
	} else {
		l.tail = nil
	}
	l.size--
	return node.value, true
}

func (l *DoublyLinkedList) PopBack() (string, bool) {
	if l.tail == nil {
		return "", false
	}
	node := l.tail
	l.tail = node.prev
	if l.tail != nil {
		l.tail.next = nil
	} else {
		l.head = nil
	}
	l.size--
	return node.value, true
}

func (l *DoublyLinkedList) DeleteBefore(target string) (string, bool) {
	node := l.findNode(target)
	if node == nil || node.prev == nil {
		return "", false
	}
	return l.unlink(node.prev), true
}

func (l *DoublyLinkedList) DeleteAfter(target string) (string, bool) {
	node := l.findNode(target)
	if node == nil || node.next == nil {
		return "", false
	}
	return l.unlink(node.next), true
}

func (l *DoublyLinkedList) DeleteValue(value string) (string, bool) {
	node := l.findNode(value)
	if node == nil {
		return "", false
	}
	return l.unlink(node), true
}

func (l *DoublyLinkedList) Find(value string) int {
	index := 0
	for node := l.head; node != nil; node = node.next {
		if node.value == value {
			return index
		}
		index++
	}
	return -1
}

func (l *DoublyLinkedList) GetAt(index int) (string, bool) {
	node := l.nodeAt(index)
	if node == nil {
		return "", false
	}
	return node.value, true
}

func (l *DoublyLinkedList) SetAt(index int, value string) bool {
	node := l.nodeAt(index)
	if node == nil {
		return false
	}
	node.value = value
	return true
}

func (l *DoublyLinkedList) Values() []string {
	result := make([]string, 0, l.size)
	for node := l.head; node != nil; node = node.next {
		result = append(result, node.value)
	}
	return result
}

func (l *DoublyLinkedList) ValuesReverse() []string {
	result := make([]string, 0, l.size)
	for node := l.tail; node != nil; node = node.prev {
		result = append(result, node.value)
	}
	return result
}

func (l *DoublyLinkedList) String() string {
	if l.size == 0 {
		return "[]"
	}
	return "[" + strings.Join(l.Values(), " <-> ") + "]"
}

func (l *DoublyLinkedList) nodeAt(index int) *doublyNode {
	if index < 0 || index >= l.size {
		return nil
	}
	node := l.head
	for i := 0; i < index; i++ {
		node = node.next
	}
	return node
}

func (l *DoublyLinkedList) findNode(value string) *doublyNode {
	for node := l.head; node != nil; node = node.next {
		if node.value == value {
			return node
		}
	}
	return nil
}

func (l *DoublyLinkedList) unlink(node *doublyNode) string {
	if node.prev != nil {
		node.prev.next = node.next
	} else {
		l.head = node.next
	}
	if node.next != nil {
		node.next.prev = node.prev
	} else {
		l.tail = node.prev
	}
	l.size--
	return node.value
}

func (l *DoublyLinkedList) MarshalJSON() ([]byte, error) {
	return json.Marshal(l.Values())
}

func (l *DoublyLinkedList) UnmarshalJSON(data []byte) error {
	var values []string
	if err := json.Unmarshal(data, &values); err != nil {
		return fmt.Errorf("dlist: %w", err)
	}
	l.Clear()
	for _, value := range values {
		l.PushBack(value)
	}
	return nil
}
