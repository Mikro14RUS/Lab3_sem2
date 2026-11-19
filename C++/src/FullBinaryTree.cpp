#include "FullBinaryTree.hpp"

#include <algorithm>
#include <limits>
#include <ostream>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace lab3 {

FullBinaryTree::FullBinaryTree() : root_(nullptr), size_(0) {}

FullBinaryTree::~FullBinaryTree() {
    deleteSubtree(root_);
}

FullBinaryTree::FullBinaryTree(const FullBinaryTree& other)
    : root_(copySubtree(other.root_)), size_(other.size_) {}

FullBinaryTree& FullBinaryTree::operator=(const FullBinaryTree& other) {
    if (this != &other) {
        FullBinaryTree copy(other);
        std::swap(root_, copy.root_);
        std::swap(size_, copy.size_);
    }
    return *this;
}

FullBinaryTree::FullBinaryTree(FullBinaryTree&& other) noexcept
    : root_(other.root_), size_(other.size_) {
    other.root_ = nullptr;
    other.size_ = 0;
}

FullBinaryTree& FullBinaryTree::operator=(FullBinaryTree&& other) noexcept {
    if (this != &other) {
        deleteSubtree(root_);
        root_ = other.root_;
        size_ = other.size_;
        other.root_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

void FullBinaryTree::deleteSubtree(Node* node) noexcept {
    if (node == nullptr) {
        return;
    }
    deleteSubtree(node->left);
    deleteSubtree(node->right);
    delete node;
}

FullBinaryTree::Node* FullBinaryTree::copySubtree(const Node* node) {
    if (node == nullptr) {
        return nullptr;
    }
    Node* copy = new Node{node->value, nullptr, nullptr};
    try {
        copy->left = copySubtree(node->left);
        copy->right = copySubtree(node->right);
    } catch (...) {
        deleteSubtree(copy);
        throw;
    }
    return copy;
}

bool FullBinaryTree::insertNode(Node*& link, int value, std::size_t& added) {
    if (link == nullptr) {
        link = new Node{value, nullptr, nullptr};
        ++added;
        return true;
    }
    if (value == link->value) {
        return false;
    }
    return insertNode(value < link->value ? link->left : link->right, value, added);
}

bool FullBinaryTree::insert(int value) {
    std::size_t added = 0;
    const bool inserted = insertNode(root_, value, added);
    size_ += added;
    return inserted;
}

bool FullBinaryTree::find(int value) const {
    const Node* node = root_;
    while (node != nullptr) {
        if (node->value == value) {
            return true;
        }
        node = value < node->value ? node->left : node->right;
    }
    return false;
}

bool FullBinaryTree::isFullNode(const Node* node) {
    if (node == nullptr) {
        return true;
    }
    if ((node->left == nullptr) != (node->right == nullptr)) {
        return false;
    }
    return isFullNode(node->left) && isFullNode(node->right);
}

bool FullBinaryTree::isFull() const {
    return isFullNode(root_);
}

bool FullBinaryTree::isBstNode(const Node* node, long long minValue, long long maxValue) {
    if (node == nullptr) {
        return true;
    }
    if (node->value <= minValue || node->value >= maxValue) {
        return false;
    }
    return isBstNode(node->left, minValue, node->value) &&
           isBstNode(node->right, node->value, maxValue);
}

bool FullBinaryTree::isBst() const {
    return isBstNode(root_, std::numeric_limits<long long>::min(),
                     std::numeric_limits<long long>::max());
}

void FullBinaryTree::clear() noexcept {
    deleteSubtree(root_);
    root_ = nullptr;
    size_ = 0;
}

std::vector<int> FullBinaryTree::bfs() const {
    std::vector<int> result;
    if (root_ == nullptr) {
        return result;
    }
    std::queue<const Node*> queue;
    queue.push(root_);
    while (!queue.empty()) {
        const Node* node = queue.front();
        queue.pop();
        result.push_back(node->value);
        if (node->left != nullptr) {
            queue.push(node->left);
        }
        if (node->right != nullptr) {
            queue.push(node->right);
        }
    }
    return result;
}

std::vector<int> FullBinaryTree::preorder() const {
    std::vector<int> result;
    std::vector<const Node*> stack;
    if (root_ != nullptr) {
        stack.push_back(root_);
    }
    while (!stack.empty()) {
        const Node* node = stack.back();
        stack.pop_back();
        result.push_back(node->value);
        if (node->right != nullptr) {
            stack.push_back(node->right);
        }
        if (node->left != nullptr) {
            stack.push_back(node->left);
        }
    }
    return result;
}

std::vector<int> FullBinaryTree::inorder() const {
    std::vector<int> result;
    std::vector<const Node*> stack;
    const Node* current = root_;
    while (current != nullptr || !stack.empty()) {
        while (current != nullptr) {
            stack.push_back(current);
            current = current->left;
        }
        current = stack.back();
        stack.pop_back();
        result.push_back(current->value);
        current = current->right;
    }
    return result;
}

std::vector<int> FullBinaryTree::postorder() const {
    std::vector<int> result;
    std::vector<const Node*> first;
    std::vector<const Node*> second;
    if (root_ != nullptr) {
        first.push_back(root_);
    }
    while (!first.empty()) {
        const Node* node = first.back();
        first.pop_back();
        second.push_back(node);
        if (node->left != nullptr) {
            first.push_back(node->left);
        }
        if (node->right != nullptr) {
            first.push_back(node->right);
        }
    }
    while (!second.empty()) {
        result.push_back(second.back()->value);
        second.pop_back();
    }
    return result;
}

void FullBinaryTree::collectPreorder(const Node* node,
                                     std::vector<std::optional<int>>& out) {
    if (node == nullptr) {
        out.emplace_back(std::nullopt);
        return;
    }
    out.emplace_back(node->value);
    collectPreorder(node->left, out);
    collectPreorder(node->right, out);
}

std::vector<std::optional<int>> FullBinaryTree::preorderWithNulls() const {
    std::vector<std::optional<int>> result;
    collectPreorder(root_, result);
    return result;
}

FullBinaryTree::Node* FullBinaryTree::buildFromPreorder(
    const std::vector<std::optional<int>>& tokens, std::size_t& position, bool& ok) {
    if (position >= tokens.size()) {
        ok = false;
        return nullptr;
    }
    if (!tokens[position].has_value()) {
        ++position;
        return nullptr;
    }
    Node* node = new Node{*tokens[position], nullptr, nullptr};
    ++position;
    node->left = buildFromPreorder(tokens, position, ok);
    if (!ok) {
        deleteSubtree(node);
        return nullptr;
    }
    node->right = buildFromPreorder(tokens, position, ok);
    if (!ok) {
        deleteSubtree(node);
        return nullptr;
    }
    return node;
}

bool FullBinaryTree::loadPreorderWithNulls(
    const std::vector<std::optional<int>>& tokens) {
    if (tokens.empty()) {
        clear();
        return true;
    }
    std::size_t position = 0;
    bool ok = true;
    Node* newRoot = buildFromPreorder(tokens, position, ok);
    if (!ok || position != tokens.size() || !isBstNode(
            newRoot, std::numeric_limits<long long>::min(),
            std::numeric_limits<long long>::max())) {
        deleteSubtree(newRoot);
        return false;
    }
    std::size_t loadedSize = 0;
    for (const auto& token : tokens) {
        if (token.has_value()) {
            ++loadedSize;
        }
    }
    deleteSubtree(root_);
    root_ = newRoot;
    size_ = loadedSize;
    return true;
}

void FullBinaryTree::prettyNode(const Node* node, const std::string& prefix,
                                bool isLast, std::ostream& out) {
    if (node == nullptr) {
        return;
    }
    out << prefix << (isLast ? "└── " : "├── ") << node->value << '\n';
    const std::string childPrefix = prefix + (isLast ? "    " : "│   ");
    const bool hasLeft = node->left != nullptr;
    const bool hasRight = node->right != nullptr;
    if (hasLeft && hasRight) {
        prettyNode(node->left, childPrefix, false, out);
        prettyNode(node->right, childPrefix, true, out);
    } else if (hasLeft) {
        prettyNode(node->left, childPrefix, true, out);
    } else if (hasRight) {
        prettyNode(node->right, childPrefix, true, out);
    }
}

std::string FullBinaryTree::toString(Traversal traversal) const {
    if (root_ == nullptr) {
        return "Дерево пусто";
    }
    if (traversal == Traversal::Pretty) {
        std::ostringstream out;
        out << "Дерево (размер: " << size_ << "):\n";
        prettyNode(root_, "", true, out);
        std::string text = out.str();
        if (!text.empty() && text.back() == '\n') {
            text.pop_back();
        }
        return text;
    }

    std::vector<int> values;
    const char* label = "BFS";
    switch (traversal) {
        case Traversal::Preorder:
            values = preorder();
            label = "Preorder";
            break;
        case Traversal::Inorder:
            values = inorder();
            label = "Inorder";
            break;
        case Traversal::Postorder:
            values = postorder();
            label = "Postorder";
            break;
        case Traversal::Bfs:
        case Traversal::Pretty:
        default:
            values = bfs();
            break;
    }
    std::ostringstream out;
    out << label << ": ";
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            out << ' ';
        }
        out << values[i];
    }
    return out.str();
}

}  // namespace lab3
