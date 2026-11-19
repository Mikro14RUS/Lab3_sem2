// Package containers содержит учебные реализации абстрактных структур
// данных. Каждая структура реализует интерфейс Container и умеет
// сериализоваться в текстовый JSON и бинарный формат проекта.
package containers

import "encoding/json"

// Container — общий полиморфный интерфейс всех структур данных.
// Он позволяет единообразно сохранять и загружать разнотипные
// контейнеры через SaveText/LoadText и SaveBinary/LoadBinary.
type Container interface {
	Type() string
	Name() string
	Len() int
	Clear()
	String() string
	json.Marshaler
	json.Unmarshaler
}

const (
	TypeArray = "array"
	TypeList  = "list"
	TypeDList = "dlist"
	TypeStack = "stack"
	TypeQueue = "queue"
	TypeHash  = "hash"
	TypeTree  = "tree"
)
