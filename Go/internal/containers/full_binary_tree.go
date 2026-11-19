package containers

import (
	"encoding/json"
	"fmt"
	"math"
	"strconv"
	"strings"
)

type treeNode struct {
	value int
	left  *treeNode
	right *treeNode
}

// FullBinaryTree — бинарное дерево поиска с проверкой свойства
// Full Binary Tree (вариант 4 ЛР1): у каждого узла 0 или 2 ребёнка.
type FullBinaryTree struct {
	name string
	root *treeNode
	size int
}

func NewFullBinaryTree(name string) *FullBinaryTree {
	return &FullBinaryTree{name: name}
}

func (t *FullBinaryTree) Type() string { return TypeTree }
func (t *FullBinaryTree) Name() string { return t.name }
func (t *FullBinaryTree) Len() int     { return t.size }

func (t *FullBinaryTree) Clear() {
	t.root = nil
	t.size = 0
}

func (t *FullBinaryTree) Insert(value int) bool {
	inserted := false
	t.root = insertTreeNode(t.root, value, &inserted)
	if inserted {
		t.size++
	}
	return inserted
}

func (t *FullBinaryTree) Find(value int) bool {
	node := t.root
	for node != nil {
		if value == node.value {
			return true
		}
		if value < node.value {
			node = node.left
		} else {
			node = node.right
		}
	}
	return false
}

func (t *FullBinaryTree) IsFull() bool { return isFullNode(t.root) }

func (t *FullBinaryTree) IsBST() bool {
	return isBSTNode(t.root, math.MinInt64, math.MaxInt64)
}

func (t *FullBinaryTree) BFS() []int {
	result := make([]int, 0, t.size)
	if t.root == nil {
		return result
	}
	queue := []*treeNode{t.root}
	for len(queue) > 0 {
		node := queue[0]
		queue = queue[1:]
		result = append(result, node.value)
		if node.left != nil {
			queue = append(queue, node.left)
		}
		if node.right != nil {
			queue = append(queue, node.right)
		}
	}
	return result
}

func (t *FullBinaryTree) Preorder() []int {
	result := make([]int, 0, t.size)
	preorderNode(t.root, &result)
	return result
}

func (t *FullBinaryTree) Inorder() []int {
	result := make([]int, 0, t.size)
	inorderNode(t.root, &result)
	return result
}

func (t *FullBinaryTree) Postorder() []int {
	result := make([]int, 0, t.size)
	postorderNode(t.root, &result)
	return result
}

// PreorderWithNulls возвращает preorder-обход с nil-маркерами
// отсутствующих детей. Формат удобен для точного восстановления формы.
func (t *FullBinaryTree) PreorderWithNulls() []*int {
	result := make([]*int, 0, t.size*2+1)
	collectPreorder(t.root, &result)
	return result
}

func (t *FullBinaryTree) LoadPreorderWithNulls(tokens []*int) bool {
	position := 0
	root := buildFromPreorder(tokens, &position)
	if root == nil && position != len(tokens) {
		return false
	}
	if position != len(tokens) {
		return false
	}
	t.root = root
	t.size = countNodes(root)
	return true
}

func (t *FullBinaryTree) String() string {
	return "BFS: [" + joinInts(t.BFS()) + "]"
}

func (t *FullBinaryTree) StringTraversal(mode string) string {
	switch strings.ToUpper(mode) {
	case "PRE":
		return "PRE: " + joinInts(t.Preorder())
	case "IN":
		return "IN: " + joinInts(t.Inorder())
	case "POST":
		return "POST: " + joinInts(t.Postorder())
	case "PRETTY":
		return t.PrettyString()
	default:
		return t.String()
	}
}

func (t *FullBinaryTree) PrettyString() string {
	if t.root == nil {
		return "Дерево пусто"
	}
	var out strings.Builder
	out.WriteString("Дерево (размер: " + strconv.Itoa(t.size) + "):\n")
	prettyNode(t.root, "", true, &out)
	return out.String()
}

func (t *FullBinaryTree) MarshalJSON() ([]byte, error) {
	return json.Marshal(t.PreorderWithNulls())
}

func (t *FullBinaryTree) UnmarshalJSON(data []byte) error {
	var tokens []*int
	if err := json.Unmarshal(data, &tokens); err != nil {
		return fmt.Errorf("tree: %w", err)
	}
	if !t.LoadPreorderWithNulls(tokens) {
		return fmt.Errorf("tree: некорректный preorder-формат")
	}
	return nil
}

func insertTreeNode(node *treeNode, value int, inserted *bool) *treeNode {
	if node == nil {
		*inserted = true
		return &treeNode{value: value}
	}
	if value < node.value {
		node.left = insertTreeNode(node.left, value, inserted)
	} else if node.value < value {
		node.right = insertTreeNode(node.right, value, inserted)
	}
	return node
}

func isFullNode(node *treeNode) bool {
	if node == nil {
		return true
	}
	if (node.left == nil) != (node.right == nil) {
		return false
	}
	return isFullNode(node.left) && isFullNode(node.right)
}

func isBSTNode(node *treeNode, minValue, maxValue int64) bool {
	if node == nil {
		return true
	}
	value := int64(node.value)
	if value <= minValue || value >= maxValue {
		return false
	}
	return isBSTNode(node.left, minValue, value) &&
		isBSTNode(node.right, value, maxValue)
}

func preorderNode(node *treeNode, out *[]int) {
	if node == nil {
		return
	}
	*out = append(*out, node.value)
	preorderNode(node.left, out)
	preorderNode(node.right, out)
}

func inorderNode(node *treeNode, out *[]int) {
	if node == nil {
		return
	}
	inorderNode(node.left, out)
	*out = append(*out, node.value)
	inorderNode(node.right, out)
}

func postorderNode(node *treeNode, out *[]int) {
	if node == nil {
		return
	}
	postorderNode(node.left, out)
	postorderNode(node.right, out)
	*out = append(*out, node.value)
}

func collectPreorder(node *treeNode, out *[]*int) {
	if node == nil {
		*out = append(*out, nil)
		return
	}
	value := node.value
	*out = append(*out, &value)
	collectPreorder(node.left, out)
	collectPreorder(node.right, out)
}

func buildFromPreorder(tokens []*int, position *int) *treeNode {
	if *position >= len(tokens) {
		*position = len(tokens) + 1
		return nil
	}
	token := tokens[*position]
	*position = *position + 1
	if token == nil {
		return nil
	}
	node := &treeNode{value: *token}
	node.left = buildFromPreorder(tokens, position)
	node.right = buildFromPreorder(tokens, position)
	return node
}

func countNodes(node *treeNode) int {
	if node == nil {
		return 0
	}
	return 1 + countNodes(node.left) + countNodes(node.right)
}

func prettyNode(node *treeNode, prefix string, isLast bool, out *strings.Builder) {
	if node == nil {
		return
	}
	connector := "├── "
	if isLast {
		connector = "└── "
	}
	out.WriteString(prefix + connector + strconv.Itoa(node.value) + "\n")
	childPrefix := prefix
	if isLast {
		childPrefix += "    "
	} else {
		childPrefix += "│   "
	}
	if node.left != nil || node.right != nil {
		prettyNode(node.left, childPrefix, node.right == nil, out)
		prettyNode(node.right, childPrefix, true, out)
	}
}

func joinInts(values []int) string {
	parts := make([]string, len(values))
	for i, value := range values {
		parts[i] = strconv.Itoa(value)
	}
	return strings.Join(parts, ", ")
}
