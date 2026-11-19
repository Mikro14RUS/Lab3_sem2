#ifndef LAB3_FULL_BINARY_TREE_HPP
#define LAB3_FULL_BINARY_TREE_HPP

#include <cstddef>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace lab3 {

// Бинарное дерево поиска с проверкой свойства Full Binary Tree
// (вариант 4 лабораторной работы №1): у каждого узла либо 0, либо 2 ребёнка.
class FullBinaryTree {
public:
    enum class Traversal { Bfs, Preorder, Inorder, Postorder, Pretty };

    FullBinaryTree();
    ~FullBinaryTree();

    FullBinaryTree(const FullBinaryTree& other);
    FullBinaryTree& operator=(const FullBinaryTree& other);
    FullBinaryTree(FullBinaryTree&& other) noexcept;
    FullBinaryTree& operator=(FullBinaryTree&& other) noexcept;

    bool insert(int value);
    bool find(int value) const;
    bool isFull() const;
    bool isBst() const;

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept;

    std::vector<int> bfs() const;
    std::vector<int> preorder() const;
    std::vector<int> inorder() const;
    std::vector<int> postorder() const;

    // preorder с явными маркерами отсутствующих детей — для сериализации.
    std::vector<std::optional<int>> preorderWithNulls() const;
    bool loadPreorderWithNulls(const std::vector<std::optional<int>>& tokens);

    std::string toString(Traversal traversal = Traversal::Bfs) const;

private:
    struct Node {
        int value;
        Node* left;
        Node* right;
    };

    static void deleteSubtree(Node* node) noexcept;
    static Node* copySubtree(const Node* node);
    static bool isFullNode(const Node* node);
    static bool isBstNode(const Node* node, long long minValue, long long maxValue);
    static void collectPreorder(const Node* node, std::vector<std::optional<int>>& out);
    static Node* buildFromPreorder(const std::vector<std::optional<int>>& tokens,
                                   std::size_t& position, bool& ok);
    static bool insertNode(Node*& link, int value, std::size_t& added);
    static void prettyNode(const Node* node, const std::string& prefix,
                           bool isLast, std::ostream& out);

    Node* root_;
    std::size_t size_;
};

}  // namespace lab3

#endif  // LAB3_FULL_BINARY_TREE_HPP
