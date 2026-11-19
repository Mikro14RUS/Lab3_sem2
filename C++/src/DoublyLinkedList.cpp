#include "DoublyLinkedList.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace lab3 {

DoublyLinkedList::DoublyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

DoublyLinkedList::~DoublyLinkedList() {
    clear();
}

DoublyLinkedList::DoublyLinkedList(const DoublyLinkedList& other)
    : head_(nullptr), tail_(nullptr), size_(0) {
    for (Node* node = other.head_; node != nullptr; node = node->next) {
        pushBack(node->value);
    }
}

DoublyLinkedList& DoublyLinkedList::operator=(const DoublyLinkedList& other) {
    if (this != &other) {
        DoublyLinkedList copy(other);
        std::swap(head_, copy.head_);
        std::swap(tail_, copy.tail_);
        std::swap(size_, copy.size_);
    }
    return *this;
}

DoublyLinkedList::DoublyLinkedList(DoublyLinkedList&& other) noexcept
    : head_(other.head_), tail_(other.tail_), size_(other.size_) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
}

DoublyLinkedList& DoublyLinkedList::operator=(DoublyLinkedList&& other) noexcept {
    if (this != &other) {
        clear();
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

void DoublyLinkedList::pushFront(const std::string& value) {
    Node* node = new Node{value, head_, nullptr};
    if (head_ != nullptr) {
        head_->prev = node;
    } else {
        tail_ = node;
    }
    head_ = node;
    ++size_;
}

void DoublyLinkedList::pushBack(const std::string& value) {
    Node* node = new Node{value, nullptr, tail_};
    if (tail_ != nullptr) {
        tail_->next = node;
    } else {
        head_ = node;
    }
    tail_ = node;
    ++size_;
}

bool DoublyLinkedList::insertBefore(const std::string& target, const std::string& value) {
    Node* targetNode = findNode(target);
    if (targetNode == nullptr) {
        return false;
    }
    Node* node = new Node{value, targetNode, targetNode->prev};
    if (targetNode->prev != nullptr) {
        targetNode->prev->next = node;
    } else {
        head_ = node;
    }
    targetNode->prev = node;
    ++size_;
    return true;
}

bool DoublyLinkedList::insertAfter(const std::string& target, const std::string& value) {
    Node* targetNode = findNode(target);
    if (targetNode == nullptr) {
        return false;
    }
    Node* node = new Node{value, targetNode->next, targetNode};
    if (targetNode->next != nullptr) {
        targetNode->next->prev = node;
    } else {
        tail_ = node;
    }
    targetNode->next = node;
    ++size_;
    return true;
}

bool DoublyLinkedList::popFront(std::string& removed) {
    if (head_ == nullptr) {
        return false;
    }
    Node* node = head_;
    removed = node->value;
    unlink(node);
    delete node;
    return true;
}

bool DoublyLinkedList::popBack(std::string& removed) {
    if (tail_ == nullptr) {
        return false;
    }
    Node* node = tail_;
    removed = node->value;
    unlink(node);
    delete node;
    return true;
}

bool DoublyLinkedList::deleteBefore(const std::string& target, std::string& removed) {
    Node* targetNode = findNode(target);
    if (targetNode == nullptr || targetNode->prev == nullptr) {
        return false;
    }
    Node* node = targetNode->prev;
    removed = node->value;
    unlink(node);
    delete node;
    return true;
}

bool DoublyLinkedList::deleteAfter(const std::string& target, std::string& removed) {
    Node* targetNode = findNode(target);
    if (targetNode == nullptr || targetNode->next == nullptr) {
        return false;
    }
    Node* node = targetNode->next;
    removed = node->value;
    unlink(node);
    delete node;
    return true;
}

bool DoublyLinkedList::deleteValue(const std::string& value, std::string& removed) {
    Node* node = findNode(value);
    if (node == nullptr) {
        return false;
    }
    removed = node->value;
    unlink(node);
    delete node;
    return true;
}

int DoublyLinkedList::find(const std::string& value) const {
    int index = 0;
    for (Node* node = head_; node != nullptr; node = node->next, ++index) {
        if (node->value == value) {
            return index;
        }
    }
    return -1;
}

bool DoublyLinkedList::getAt(std::size_t index, std::string& value) const {
    Node* node = nodeAt(index);
    if (node == nullptr) {
        return false;
    }
    value = node->value;
    return true;
}

void DoublyLinkedList::clear() noexcept {
    while (head_ != nullptr) {
        Node* node = head_;
        head_ = head_->next;
        delete node;
    }
    tail_ = nullptr;
    size_ = 0;
}

std::string DoublyLinkedList::toString(bool reverse) const {
    if (head_ == nullptr) {
        return "NULL";
    }
    std::ostringstream out;
    if (!reverse) {
        for (Node* node = head_; node != nullptr; node = node->next) {
            out << node->value << " <-> ";
        }
    } else {
        for (Node* node = tail_; node != nullptr; node = node->prev) {
            out << node->value << " <-> ";
        }
    }
    out << "NULL";
    return out.str();
}

std::vector<std::string> DoublyLinkedList::toVector() const {
    std::vector<std::string> values;
    values.reserve(size_);
    for (Node* node = head_; node != nullptr; node = node->next) {
        values.push_back(node->value);
    }
    return values;
}

void DoublyLinkedList::forEach(
    const std::function<void(const std::string&)>& visitor) const {
    for (Node* node = head_; node != nullptr; node = node->next) {
        visitor(node->value);
    }
}

DoublyLinkedList::Node* DoublyLinkedList::nodeAt(std::size_t index) const {
    if (index >= size_) {
        return nullptr;
    }
    Node* node = head_;
    for (std::size_t i = 0; i < index; ++i) {
        node = node->next;
    }
    return node;
}

DoublyLinkedList::Node* DoublyLinkedList::findNode(const std::string& value) const {
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node->value == value) {
            return node;
        }
    }
    return nullptr;
}

void DoublyLinkedList::unlink(Node* node) {
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        head_ = node->next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        tail_ = node->prev;
    }
    --size_;
}

}  // namespace lab3
