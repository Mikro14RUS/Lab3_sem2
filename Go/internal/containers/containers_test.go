package containers

import (
	"encoding/json"
	"reflect"
	"testing"
)

func assertStrings(t *testing.T, got, want []string) {
	t.Helper()
	if !reflect.DeepEqual(got, want) {
		t.Fatalf("получено %v, ожидалось %v", got, want)
	}
}

func assertInts(t *testing.T, got, want []int) {
	t.Helper()
	if !reflect.DeepEqual(got, want) {
		t.Fatalf("получено %v, ожидалось %v", got, want)
	}
}

func roundTrip(t *testing.T, source, target interface{}) {
	t.Helper()
	data, err := json.Marshal(source)
	if err != nil {
		t.Fatalf("Marshal: %v", err)
	}
	if err := json.Unmarshal(data, target); err != nil {
		t.Fatalf("Unmarshal: %v", err)
	}
}

func TestArrayBasic(t *testing.T) {
	a := NewArray("arr")
	if a.Type() != TypeArray || a.Name() != "arr" || a.Len() != 0 {
		t.Fatalf("неверные метаданные массива")
	}
	if a.String() != "[]" {
		t.Fatalf("пустой массив: %q", a.String())
	}

	a.PushBack("b")
	a.PushFront("a")
	a.PushBack("d")
	if !a.InsertAt(2, "c") {
		t.Fatal("InsertAt(2) должен succeed")
	}
	assertStrings(t, a.Values(), []string{"a", "b", "c", "d"})
	if a.String() != "[a, b, c, d]" {
		t.Fatalf("String: %q", a.String())
	}

	if value, ok := a.GetAt(2); !ok || value != "c" {
		t.Fatalf("GetAt(2) = %q, %v", value, ok)
	}
	if !a.SetAt(2, "C") {
		t.Fatal("SetAt должен succeed")
	}
	if a.Values()[2] != "C" {
		t.Fatal("SetAt не изменил значение")
	}
	if value, ok := a.RemoveAt(1); !ok || value != "b" {
		t.Fatalf("RemoveAt(1) = %q, %v", value, ok)
	}
	if value, ok := a.PopFront(); !ok || value != "a" {
		t.Fatalf("PopFront = %q, %v", value, ok)
	}
	if value, ok := a.PopBack(); !ok || value != "d" {
		t.Fatalf("PopBack = %q, %v", value, ok)
	}
	if a.Len() != 1 || a.Values()[0] != "C" {
		t.Fatalf("после удалений: %v", a.Values())
	}

	a.Clear()
	if a.Len() != 0 || a.String() != "[]" {
		t.Fatal("Clear не очистил массив")
	}
	if _, ok := a.PopBack(); ok {
		t.Fatal("PopBack на пустом массиве")
	}
	if _, ok := a.PopFront(); ok {
		t.Fatal("PopFront на пустом массиве")
	}
	if _, ok := a.RemoveAt(0); ok {
		t.Fatal("RemoveAt на пустом массиве")
	}
	if _, ok := a.GetAt(0); ok {
		t.Fatal("GetAt на пустом массиве")
	}
	if a.SetAt(0, "x") {
		t.Fatal("SetAt на пустом массиве")
	}
	if a.InsertAt(1, "x") {
		t.Fatal("InsertAt за границей")
	}
	if a.InsertAt(-1, "x") {
		t.Fatal("InsertAt с отрицательным индексом")
	}
}

func TestArrayJSON(t *testing.T) {
	var empty Array
	data, err := empty.MarshalJSON()
	if err != nil || string(data) != "[]" {
		t.Fatalf("MarshalJSON zero value: %s, %v", data, err)
	}

	source := NewArray("arr")
	source.PushBack("one")
	source.PushBack("two")
	target := NewArray("other")
	roundTrip(t, source, target)
	assertStrings(t, target.Values(), []string{"one", "two"})

	if err := target.UnmarshalJSON([]byte(`"bad"`)); err == nil {
		t.Fatal("ожидалась ошибка для не-массива")
	}
	if err := target.UnmarshalJSON([]byte(`null`)); err != nil {
		t.Fatalf("null: %v", err)
	}
	if target.Len() != 0 {
		t.Fatal("null должен дать пустой массив")
	}
}

func TestSinglyLinkedListBasic(t *testing.T) {
	l := NewSinglyLinkedList("list")
	if l.Type() != TypeList || l.Name() != "list" || l.Len() != 0 || l.String() != "[]" {
		t.Fatal("неверные метаданные списка")
	}
	if _, ok := l.PopFront(); ok {
		t.Fatal("PopFront пустого списка")
	}
	if _, ok := l.PopBack(); ok {
		t.Fatal("PopBack пустого списка")
	}
	if l.InsertBefore("x", "y") || l.InsertAfter("x", "y") {
		t.Fatal("вставка в пустой список")
	}

	l.PushBack("b")
	l.PushFront("a")
	l.PushBack("d")
	if !l.InsertBefore("b", "a2") {
		t.Fatal("InsertBefore середины")
	}
	if !l.InsertAfter("d", "e") {
		t.Fatal("InsertAfter хвоста")
	}
	assertStrings(t, l.Values(), []string{"a", "a2", "b", "d", "e"})
	if l.String() != "[a -> a2 -> b -> d -> e]" {
		t.Fatalf("String: %q", l.String())
	}
	if l.Find("b") != 2 || l.Find("missing") != -1 {
		t.Fatal("Find вернул неверный индекс")
	}
	if value, ok := l.GetAt(2); !ok || value != "b" {
		t.Fatalf("GetAt = %q, %v", value, ok)
	}
	if !l.SetAt(2, "B") || l.Values()[2] != "B" {
		t.Fatal("SetAt не сработал")
	}
	if _, ok := l.GetAt(-1); ok {
		t.Fatal("GetAt(-1)")
	}
	if _, ok := l.GetAt(100); ok {
		t.Fatal("GetAt(100)")
	}
	if l.SetAt(-1, "x") || l.SetAt(100, "x") {
		t.Fatal("SetAt с неверным индексом")
	}

	if value, ok := l.DeleteBefore("B"); !ok || value != "a2" {
		t.Fatalf("DeleteBefore = %q, %v", value, ok)
	}
	if value, ok := l.DeleteAfter("B"); !ok || value != "d" {
		t.Fatalf("DeleteAfter = %q, %v", value, ok)
	}
	assertStrings(t, l.Values(), []string{"a", "B", "e"})

	if value, ok := l.DeleteValue("B"); !ok || value != "B" {
		t.Fatalf("DeleteValue середины = %q, %v", value, ok)
	}
	if value, ok := l.DeleteValue("a"); !ok || value != "a" {
		t.Fatalf("DeleteValue головы = %q, %v", value, ok)
	}
	if value, ok := l.DeleteValue("e"); !ok || value != "e" {
		t.Fatalf("DeleteValue хвоста = %q, %v", value, ok)
	}
	if _, ok := l.DeleteValue("missing"); ok {
		t.Fatal("DeleteValue отсутствующего")
	}
	if _, ok := l.DeleteBefore("missing"); ok {
		t.Fatal("DeleteBefore отсутствующего")
	}
	if _, ok := l.DeleteAfter("missing"); ok {
		t.Fatal("DeleteAfter отсутствующего")
	}

	l.PushBack("x")
	if !l.InsertBefore("x", "head") {
		t.Fatal("InsertBefore головы")
	}
	if l.Values()[0] != "head" {
		t.Fatal("InsertBefore не вставил в голову")
	}
	if !l.InsertAfter("x", "tail") || l.Values()[len(l.Values())-1] != "tail" {
		t.Fatal("InsertAfter не обновил tail")
	}
	if _, ok := l.DeleteAfter("x"); !ok {
		t.Fatal("DeleteAfter хвоста")
	}
	if _, ok := l.DeleteAfter("x"); ok {
		t.Fatal("DeleteAfter хвоста должен вернуть false")
	}
	l.Clear()
	if l.Len() != 0 || l.Values() != nil && len(l.Values()) != 0 {
		t.Fatal("Clear")
	}
}

func TestSinglyLinkedListPopBack(t *testing.T) {
	l := NewSinglyLinkedList("list")

	if value, ok := l.PopBack(); ok || value != "" {
		t.Fatalf("PopBack пустого = %q, %v", value, ok)
	}

	l.PushBack("one")
	l.PushBack("two")
	l.PushBack("three")

	if value, ok := l.PopBack(); !ok || value != "three" || l.Len() != 2 {
		t.Fatalf("PopBack из трёх элементов = %q, %v, len=%d", value, ok, l.Len())
	}
	assertStrings(t, l.Values(), []string{"one", "two"})
	if l.tail == nil || l.tail.value != "two" {
		t.Fatal("PopBack не обновил tail")
	}

	if value, ok := l.PopBack(); !ok || value != "two" || l.Len() != 1 {
		t.Fatalf("PopBack из двух элементов = %q, %v, len=%d", value, ok, l.Len())
	}
	assertStrings(t, l.Values(), []string{"one"})
	if l.head == nil || l.head.next != nil {
		t.Fatal("PopBack не разорвал связь с удалённым хвостом")
	}
	if l.tail == nil || l.tail.value != "one" {
		t.Fatal("PopBack не обновил tail после второго удаления")
	}

	if value, ok := l.PopBack(); !ok || value != "one" || l.Len() != 0 {
		t.Fatalf("PopBack единственного элемента = %q, %v, len=%d", value, ok, l.Len())
	}
	if l.head != nil || l.tail != nil {
		t.Fatal("PopBack единственного элемента не очистил список")
	}

	if value, ok := l.PopBack(); ok || value != "" {
		t.Fatalf("PopBack пустого после удаления = %q, %v", value, ok)
	}

	l.PushBack("after")
	if value, ok := l.PopBack(); !ok || value != "after" || l.Len() != 0 {
		t.Fatalf("PopBack после повторного добавления = %q, %v, len=%d", value, ok, l.Len())
	}
}

func TestSinglyLinkedListJSON(t *testing.T) {
	source := NewSinglyLinkedList("list")
	source.PushBack("one")
	source.PushBack("two")
	target := NewSinglyLinkedList("other")
	roundTrip(t, source, target)
	assertStrings(t, target.Values(), []string{"one", "two"})
	if err := target.UnmarshalJSON([]byte(`42`)); err == nil {
		t.Fatal("ожидалась ошибка")
	}
}

func TestDoublyLinkedListBasic(t *testing.T) {
	l := NewDoublyLinkedList("dlist")
	if l.Type() != TypeDList || l.Name() != "dlist" || l.Len() != 0 || l.String() != "[]" {
		t.Fatal("неверные метаданные списка")
	}
	if _, ok := l.PopFront(); ok {
		t.Fatal("PopFront пустого списка")
	}
	if _, ok := l.PopBack(); ok {
		t.Fatal("PopBack пустого списка")
	}
	if l.InsertBefore("x", "y") || l.InsertAfter("x", "y") {
		t.Fatal("вставка в пустой список")
	}

	l.PushBack("b")
	l.PushFront("a")
	l.PushBack("d")
	if !l.InsertBefore("b", "a2") {
		t.Fatal("InsertBefore середины")
	}
	if !l.InsertAfter("d", "e") {
		t.Fatal("InsertAfter хвоста")
	}
	if !l.InsertBefore("a", "head") {
		t.Fatal("InsertBefore головы")
	}
	assertStrings(t, l.Values(), []string{"head", "a", "a2", "b", "d", "e"})
	assertStrings(t, l.ValuesReverse(), []string{"e", "d", "b", "a2", "a", "head"})
	if l.Find("b") != 3 || l.Find("missing") != -1 {
		t.Fatal("Find вернул неверный индекс")
	}
	if value, ok := l.GetAt(1); !ok || value != "a" {
		t.Fatalf("GetAt = %q, %v", value, ok)
	}
	if !l.SetAt(1, "A") || l.Values()[1] != "A" {
		t.Fatal("SetAt не сработал")
	}
	if _, ok := l.GetAt(-1); ok {
		t.Fatal("GetAt(-1)")
	}
	if _, ok := l.GetAt(100); ok {
		t.Fatal("GetAt(100)")
	}
	if l.SetAt(-1, "x") || l.SetAt(100, "x") {
		t.Fatal("SetAt с неверным индексом")
	}

	if value, ok := l.DeleteBefore("b"); !ok || value != "a2" {
		t.Fatalf("DeleteBefore = %q, %v", value, ok)
	}
	if value, ok := l.DeleteAfter("b"); !ok || value != "d" {
		t.Fatalf("DeleteAfter = %q, %v", value, ok)
	}
	if value, ok := l.DeleteValue("b"); !ok || value != "b" {
		t.Fatalf("DeleteValue = %q, %v", value, ok)
	}
	if _, ok := l.DeleteBefore("missing"); ok {
		t.Fatal("DeleteBefore отсутствующего")
	}
	if _, ok := l.DeleteAfter("missing"); ok {
		t.Fatal("DeleteAfter отсутствующего")
	}
	if _, ok := l.DeleteValue("missing"); ok {
		t.Fatal("DeleteValue отсутствующего")
	}
	if _, ok := l.DeleteBefore("head"); ok {
		t.Fatal("DeleteBefore головы")
	}
	if _, ok := l.DeleteAfter("e"); ok {
		t.Fatal("DeleteAfter хвоста")
	}

	l.Clear()
	l.PushBack("only")
	if value, ok := l.PopBack(); !ok || value != "only" || l.Len() != 0 {
		t.Fatalf("PopBack единственного = %q, %v", value, ok)
	}
	l.PushBack("one")
	l.PushBack("two")
	if value, ok := l.PopFront(); !ok || value != "one" || l.Len() != 1 {
		t.Fatalf("PopFront = %q, %v", value, ok)
	}
	if l.Values()[0] != "two" {
		t.Fatal("PopFront повредил список")
	}
	l.Clear()
	if l.Len() != 0 {
		t.Fatal("Clear")
	}
}

func TestDoublyLinkedListJSON(t *testing.T) {
	source := NewDoublyLinkedList("dlist")
	source.PushBack("one")
	source.PushBack("two")
	target := NewDoublyLinkedList("other")
	roundTrip(t, source, target)
	assertStrings(t, target.Values(), []string{"one", "two"})
	if err := target.UnmarshalJSON([]byte(`true`)); err == nil {
		t.Fatal("ожидалась ошибка")
	}
}

func TestStack(t *testing.T) {
	s := NewStack("stack")
	if s.Type() != TypeStack || s.Name() != "stack" || s.Len() != 0 || s.String() != "[]" {
		t.Fatal("неверные метаданные стека")
	}
	if _, ok := s.Pop(); ok {
		t.Fatal("Pop пустого стека")
	}
	if _, ok := s.Peek(); ok {
		t.Fatal("Peek пустого стека")
	}
	s.Push("a")
	s.Push("b")
	s.Push("c")
	if s.Len() != 3 {
		t.Fatal("Len")
	}
	if value, ok := s.Peek(); !ok || value != "c" {
		t.Fatalf("Peek = %q, %v", value, ok)
	}
	assertStrings(t, s.Values(), []string{"c", "b", "a"})
	if s.String() != "[c, b, a]" {
		t.Fatalf("String: %q", s.String())
	}
	if value, ok := s.Pop(); !ok || value != "c" {
		t.Fatalf("Pop = %q, %v", value, ok)
	}
	s.Clear()
	if s.Len() != 0 || s.String() != "[]" {
		t.Fatal("Clear")
	}
}

func TestStackJSON(t *testing.T) {
	var empty Stack
	data, err := empty.MarshalJSON()
	if err != nil || string(data) != "[]" {
		t.Fatalf("MarshalJSON zero value: %s, %v", data, err)
	}
	source := NewStack("stack")
	source.Push("one")
	source.Push("two")
	target := NewStack("other")
	roundTrip(t, source, target)
	assertStrings(t, target.Values(), []string{"two", "one"})
	if err := target.UnmarshalJSON([]byte(`"bad"`)); err == nil {
		t.Fatal("ожидалась ошибка")
	}
	if err := target.UnmarshalJSON([]byte(`null`)); err != nil || target.Len() != 0 {
		t.Fatalf("null: %v", err)
	}
}

func TestQueue(t *testing.T) {
	q := NewQueue("queue")
	if q.Type() != TypeQueue || q.Name() != "queue" || q.Len() != 0 {
		t.Fatal("неверные метаданные очереди")
	}
	if q.String() != "начало -> <- конец" {
		t.Fatalf("String пустой очереди: %q", q.String())
	}
	if _, ok := q.Pop(); ok {
		t.Fatal("Pop пустой очереди")
	}
	if _, ok := q.Front(); ok {
		t.Fatal("Front пустой очереди")
	}
	q.Push("a")
	q.Push("b")
	q.Push("c")
	if value, ok := q.Front(); !ok || value != "a" {
		t.Fatalf("Front = %q, %v", value, ok)
	}
	assertStrings(t, q.Values(), []string{"a", "b", "c"})
	if q.String() != "начало -> a b c <- конец" {
		t.Fatalf("String: %q", q.String())
	}
	if value, ok := q.Pop(); !ok || value != "a" {
		t.Fatalf("Pop = %q, %v", value, ok)
	}
	if value, ok := q.Pop(); !ok || value != "b" {
		t.Fatalf("Pop = %q, %v", value, ok)
	}
	q.Push("d")
	q.Push("e")
	q.Push("f")
	for q.Len() > 0 {
		if _, ok := q.Pop(); !ok {
			t.Fatal("Pop во время очистки")
		}
	}
	if q.Len() != 0 || q.Values() == nil || len(q.Values()) != 0 {
		t.Fatal("очередь должна быть пуста")
	}
	q.Push("x")
	q.Clear()
	if q.Len() != 0 {
		t.Fatal("Clear")
	}
}

func TestQueueJSON(t *testing.T) {
	source := NewQueue("queue")
	source.Push("one")
	source.Push("two")
	target := NewQueue("other")
	roundTrip(t, source, target)
	assertStrings(t, target.Values(), []string{"one", "two"})
	if err := target.UnmarshalJSON([]byte(`{}`)); err == nil {
		t.Fatal("ожидалась ошибка")
	}
}

func TestHashTable(t *testing.T) {
	h := NewHashTable("hash", 1)
	if h.Type() != TypeHash || h.Name() != "hash" || h.Len() != 0 {
		t.Fatal("неверные метаданные таблицы")
	}
	if h.BucketCount() != 2 {
		t.Fatalf("минимальное число корзин: %d", h.BucketCount())
	}
	if h.String() != "{}" {
		t.Fatalf("String пустой таблицы: %q", h.String())
	}
	if h.LoadFactor() != 0 {
		t.Fatal("LoadFactor пустой таблицы")
	}
	if _, ok := h.Get("none"); ok {
		t.Fatal("Get отсутствующего ключа")
	}
	if _, ok := h.Remove("none"); ok {
		t.Fatal("Remove отсутствующего ключа")
	}
	if h.Contains("none") {
		t.Fatal("Contains отсутствующего ключа")
	}

	if !h.Put("b", "2") {
		t.Fatal("Put нового ключа")
	}
	if h.Put("b", "two") {
		t.Fatal("Put существующего ключа должен вернуть false")
	}
	if value, ok := h.Get("b"); !ok || value != "two" {
		t.Fatalf("Get = %q, %v", value, ok)
	}
	h.Put("a", "1")
	h.Put("c", "3")
	if h.Len() != 3 {
		t.Fatal("Len")
	}
	if h.LoadFactor() <= 0 {
		t.Fatal("LoadFactor")
	}
	if h.String() != "{a=1, b=two, c=3}" {
		t.Fatalf("String: %q", h.String())
	}
	if h.Collisions() < 0 {
		t.Fatal("Collisions")
	}

	for i := 0; i < 100; i++ {
		h.Put(string(rune('A'+i%26))+string(rune('0'+i/26)), "v")
	}
	if h.BucketCount() <= 2 {
		t.Fatal("таблица должна была расшириться")
	}
	if _, ok := h.Remove("b"); !ok {
		t.Fatal("Remove существующего ключа")
	}
	if h.Contains("b") {
		t.Fatal("Contains удалённого ключа")
	}

	h.Clear()
	if h.Len() != 0 || h.Collisions() != 0 || h.String() != "{}" {
		t.Fatal("Clear")
	}

	var zero HashTable
	if zero.LoadFactor() != 0 {
		t.Fatal("LoadFactor zero value")
	}
}

func TestHashTableJSON(t *testing.T) {
	source := NewHashTable("hash", 4)
	source.Put("b", "2")
	source.Put("a", "1")
	target := NewHashTable("other", 2)
	roundTrip(t, source, target)
	if target.Len() != 2 || !target.Contains("a") || !target.Contains("b") {
		t.Fatalf("после JSON: %s", target.String())
	}
	if err := target.UnmarshalJSON([]byte(`"bad"`)); err == nil {
		t.Fatal("ожидалась ошибка")
	}
}

func TestFullBinaryTree(t *testing.T) {
	tree := NewFullBinaryTree("tree")
	if tree.Type() != TypeTree || tree.Name() != "tree" || tree.Len() != 0 {
		t.Fatal("неверные метаданные дерева")
	}
	if tree.String() != "BFS: []" {
		t.Fatalf("String пустого дерева: %q", tree.String())
	}
	if tree.PrettyString() != "Дерево пусто" {
		t.Fatalf("PrettyString пустого дерева: %q", tree.PrettyString())
	}
	if !tree.IsFull() || !tree.IsBST() {
		t.Fatal("пустое дерево должно быть full и BST")
	}
	if tree.Find(1) {
		t.Fatal("Find в пустом дереве")
	}
	if len(tree.BFS()) != 0 || len(tree.Preorder()) != 0 || len(tree.Inorder()) != 0 ||
		len(tree.Postorder()) != 0 {
		t.Fatal("обходы пустого дерева должны быть пусты")
	}

	for _, value := range []int{50, 30, 70, 20, 40, 60, 80} {
		if !tree.Insert(value) {
			t.Fatalf("Insert(%d)", value)
		}
	}
	if tree.Insert(50) {
		t.Fatal("повторный Insert должен вернуть false")
	}
	if tree.Len() != 7 {
		t.Fatalf("Len = %d", tree.Len())
	}
	if !tree.IsFull() || !tree.IsBST() {
		t.Fatal("дерево должно быть full и BST")
	}
	assertInts(t, tree.BFS(), []int{50, 30, 70, 20, 40, 60, 80})
	assertInts(t, tree.Preorder(), []int{50, 30, 20, 40, 70, 60, 80})
	assertInts(t, tree.Inorder(), []int{20, 30, 40, 50, 60, 70, 80})
	assertInts(t, tree.Postorder(), []int{20, 40, 30, 60, 80, 70, 50})
	if !tree.Find(60) || tree.Find(61) {
		t.Fatal("Find")
	}
	if tree.String() != "BFS: [50, 30, 70, 20, 40, 60, 80]" {
		t.Fatalf("String: %q", tree.String())
	}
	if tree.StringTraversal("PRE") != "PRE: 50, 30, 20, 40, 70, 60, 80" {
		t.Fatalf("PRE: %q", tree.StringTraversal("PRE"))
	}
	if tree.StringTraversal("IN") != "IN: 20, 30, 40, 50, 60, 70, 80" {
		t.Fatalf("IN: %q", tree.StringTraversal("IN"))
	}
	if tree.StringTraversal("POST") != "POST: 20, 40, 30, 60, 80, 70, 50" {
		t.Fatalf("POST: %q", tree.StringTraversal("POST"))
	}
	if tree.StringTraversal("BFS") != tree.String() {
		t.Fatal("BFS через StringTraversal")
	}
	if tree.StringTraversal("PRETTY") == "" {
		t.Fatal("PRETTY не должен быть пустым")
	}
	nulls := tree.PreorderWithNulls()
	if len(nulls) != 15 {
		t.Fatalf("PreorderWithNulls: %d элементов", len(nulls))
	}

	if !tree.Insert(10) {
		t.Fatal("Insert(10)")
	}
	if !tree.Insert(15) {
		t.Fatal("Insert(15)")
	}
	if tree.IsFull() {
		t.Fatal("дерево с одним ребёнком не full")
	}

	tree.Clear()
	if tree.Len() != 0 || tree.String() != "BFS: []" {
		t.Fatal("Clear")
	}

	var zero FullBinaryTree
	if !zero.IsFull() || !zero.IsBST() {
		t.Fatal("zero value")
	}
}

func TestFullBinaryTreeSerialization(t *testing.T) {
	source := NewFullBinaryTree("tree")
	for _, value := range []int{5, 3, 8, 1, 4, 7, 9} {
		source.Insert(value)
	}
	target := NewFullBinaryTree("other")
	roundTrip(t, source, target)
	assertInts(t, target.Preorder(), source.Preorder())
	if target.Len() != source.Len() {
		t.Fatal("Len после загрузки")
	}

	empty := NewFullBinaryTree("empty")
	emptyTarget := NewFullBinaryTree("target")
	roundTrip(t, empty, emptyTarget)
	if emptyTarget.Len() != 0 {
		t.Fatal("пустое дерево не загрузилось")
	}

	if err := target.UnmarshalJSON([]byte(`"bad"`)); err == nil {
		t.Fatal("ожидалась ошибка формата")
	}
	if err := target.UnmarshalJSON([]byte(`[1,2]`)); err == nil {
		t.Fatal("ожидалась ошибка неполного preorder")
	}
	if !target.LoadPreorderWithNulls([]*int{nil}) || target.Len() != 0 {
		t.Fatal("загрузка пустого preorder")
	}
	if target.LoadPreorderWithNulls(nil) {
		t.Fatal("пустой список токенов должен отклоняться")
	}
	one := 1
	if !target.LoadPreorderWithNulls([]*int{&one, nil, nil}) || target.Len() != 1 {
		t.Fatal("загрузка одного узла")
	}
	if target.LoadPreorderWithNulls([]*int{&one}) {
		t.Fatal("неполный preorder должен отклоняться")
	}
}
