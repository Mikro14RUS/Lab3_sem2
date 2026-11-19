#include "SinglyLinkedList.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace lab3 {

SinglyLinkedList::SinglyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

SinglyLinkedList::~SinglyLinkedList() {
    clear();
}

SinglyLinkedList::SinglyLinkedList(const SinglyLinkedList& other)
    : head_(nullptr), tail_(nullptr), size_(0) {
    for (Node* node = other.head_; node != nullptr; node = node->next) {
        pushBack(node->value);
    }
}

SinglyLinkedList& SinglyLinkedList::operator=(const SinglyLinkedList& other) {
    if (this != &other) {
        SinglyLinkedList copy(other);
        std::swap(head_, copy.head_);
        std::swap(tail_, copy.tail_);
        std::swap(size_, copy.size_);
    }
    return *this;
}

SinglyLinkedList::SinglyLinkedList(SinglyLinkedList&& other) noexcept
    : head_(other.head_), tail_(other.tail_), size_(other.size_) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
}

SinglyLinkedList& SinglyLinkedList::operator=(SinglyLinkedList&& other) noexcept {
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

void SinglyLinkedList::pushFront(const std::string& value) {
    Node* node = new Node{value, head_};
    head_ = node;
    if (tail_ == nullptr) {
        tail_ = node;
    }
    ++size_;
}

void SinglyLinkedList::pushBack(const std::string& value) {
    Node* node = new Node{value, nullptr};
    if (tail_ != nullptr) {
        tail_->next = node;
    } else {
        head_ = node;
    }
    tail_ = node;
    ++size_;
}

bool SinglyLinkedList::insertBefore(const std::string& target, const std::string& value) {
    if (head_ == nullptr) {
        return false;
    }
    if (head_->value == target) {
        pushFront(value);
        return true;
    }
    Node* previous = head_;
    while (previous->next != nullptr && previous->next->value != target) {
        previous = previous->next;
    }
    if (previous->next == nullptr) {
        return false;
    }
    Node* node = new Node{value, previous->next};
    previous->next = node;
    ++size_;
    return true;
}

bool SinglyLinkedList::insertAfter(const std::string& target, const std::string& value) {
    Node* targetNode = findNode(target);
    if (targetNode == nullptr) {
        return false;
    }
    Node* node = new Node{value, targetNode->next};
    targetNode->next = node;
    if (tail_ == targetNode) {
        tail_ = node;
    }
    ++size_;
    return true;
}

bool SinglyLinkedList::popFront(std::string& removed) {
    if (head_ == nullptr) {
        return false;
    }
    Node* node = head_;
    removed = node->value;
    head_ = node->next;
    if (head_ == nullptr) {
        tail_ = nullptr;
    }
    delete node;
    --size_;
    return true;
}

bool SinglyLinkedList::popBack(std::string& removed) {
    if (head_ == nullptr) {
        return false;
    }
    if (head_ == tail_) {
        return popFront(removed);
    }
    Node* previous = head_;
    while (previous->next != tail_) {
        previous = previous->next;
    }
    removed = tail_->value;
    delete tail_;
    tail_ = previous;
    tail_->next = nullptr;
    --size_;
    return true;
}

bool SinglyLinkedList::deleteBefore(const std::string& target, std::string& removed) {
    if (head_ == nullptr || head_->value == target) {
        return false;
    }
    if (head_->next != nullptr && head_->next->value == target) {
        return popFront(removed);
    }
    Node* previous = head_;
    while (previous->next != nullptr && previous->next->next != nullptr &&
           previous->next->next->value != target) {
        previous = previous->next;
    }
    if (previous->next == nullptr || previous->next->next == nullptr) {
        return false;
    }
    Node* node = previous->next;
    removed = node->value;
    previous->next = node->next;
    delete node;
    --size_;
    return true;
}

bool SinglyLinkedList::deleteAfter(const std::string& target, std::string& removed) {
    Node* targetNode = findNode(target);
    if (targetNode == nullptr || targetNode->next == nullptr) {
        return false;
    }
    Node* node = targetNode->next;
    removed = node->value;
    targetNode->next = node->next;
    if (node == tail_) {
        tail_ = targetNode;
    }
    delete node;
    --size_;
    return true;
}

bool SinglyLinkedList::deleteValue(const std::string& value, std::string& removed) {
    if (head_ == nullptr) {
        return false;
    }
    if (head_->value == value) {
        return popFront(removed);
    }
    Node* previous = head_;
    while (previous->next != nullptr && previous->next->value != value) {
        previous = previous->next;
    }
    if (previous->next == nullptr) {
        return false;
    }
    Node* node = previous->next;
    removed = node->value;
    previous->next = node->next;
    if (node == tail_) {
        tail_ = previous;
    }
    delete node;
    --size_;
    return true;
}

int SinglyLinkedList::find(const std::string& value) const {
    int index = 0;
    for (Node* node = head_; node != nullptr; node = node->next, ++index) {
        if (node->value == value) {
            return index;
        }
    }
    return -1;
}

bool SinglyLinkedList::getAt(std::size_t index, std::string& value) const {
    Node* node = nodeAt(index);
    if (node == nullptr) {
        return false;
    }
    value = node->value;
    return true;
}

void SinglyLinkedList::clear() noexcept {
    while (head_ != nullptr) {
        Node* node = head_;
        head_ = head_->next;
        delete node;
    }
    tail_ = nullptr;
    size_ = 0;
}

std::string SinglyLinkedList::toString() const {
    if (head_ == nullptr) {
        return "NULL";
    }
    std::ostringstream out;
    for (Node* node = head_; node != nullptr; node = node->next) {
        out << node->value << " -> ";
    }
    out << "NULL";
    return out.str();
}

std::vector<std::string> SinglyLinkedList::toVector() const {
    std::vector<std::string> values;
    values.reserve(size_);
    for (Node* node = head_; node != nullptr; node = node->next) {
        values.push_back(node->value);
    }
    return values;
}

void SinglyLinkedList::forEach(
    const std::function<void(const std::string&)>& visitor) const {
    for (Node* node = head_; node != nullptr; node = node->next) {
        visitor(node->value);
    }
}

SinglyLinkedList::Node* SinglyLinkedList::nodeAt(std::size_t index) const {
    if (index >= size_) {
        return nullptr;
    }
    Node* node = head_;
    for (std::size_t i = 0; i < index; ++i) {
        node = node->next;
    }
    return node;
}

SinglyLinkedList::Node* SinglyLinkedList::findNode(const std::string& value) const {
    for (Node* node = head_; node != nullptr; node = node->next) {
        if (node->value == value) {
            return node;
        }
    }
    return nullptr;
}

}  // namespace lab3
