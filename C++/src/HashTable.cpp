#include "HashTable.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace lab3 {

HashTable::HashTable(std::size_t initialCapacity)
    : buckets_(new Node*[initialCapacity == 0 ? 1 : initialCapacity]()),
      capacity_(initialCapacity == 0 ? 1 : initialCapacity),
      size_(0),
      collisions_(0) {}

HashTable::~HashTable() {
    clear();
    delete[] buckets_;
}

std::size_t HashTable::hashOf(const std::string& key) {
    return std::hash<std::string>{}(key);
}

std::size_t HashTable::bucketIndex(const std::string& key) const {
    return hashOf(key) % capacity_;
}

HashTable::Node* HashTable::findNode(const std::string& key) const {
    Node* node = buckets_[bucketIndex(key)];
    while (node != nullptr) {
        if (node->key == key) {
            return node;
        }
        node = node->next;
    }
    return nullptr;
}

void HashTable::rehash(std::size_t newCapacity) {
    Node** newBuckets = new Node*[newCapacity]();
    for (std::size_t i = 0; i < capacity_; ++i) {
        Node* node = buckets_[i];
        while (node != nullptr) {
            Node* next = node->next;
            std::size_t index = hashOf(node->key) % newCapacity;
            node->next = newBuckets[index];
            newBuckets[index] = node;
            node = next;
        }
    }
    delete[] buckets_;
    buckets_ = newBuckets;
    capacity_ = newCapacity;
    collisions_ = 0;
    for (std::size_t i = 0; i < capacity_; ++i) {
        std::size_t chain = 0;
        for (Node* node = buckets_[i]; node != nullptr; node = node->next) {
            ++chain;
        }
        if (chain > 1) {
            collisions_ += chain - 1;
        }
    }
}

bool HashTable::put(const std::string& key, const std::string& value) {
    Node* existing = findNode(key);
    if (existing != nullptr) {
        existing->value = value;
        return false;
    }
    if ((size_ + 1) * 4 > capacity_ * 3) {
        rehash(capacity_ * 2 + 1);
    }
    std::size_t index = bucketIndex(key);
    if (buckets_[index] != nullptr) {
        ++collisions_;
    }
    Node* node = new Node{key, value, buckets_[index]};
    buckets_[index] = node;
    ++size_;
    return true;
}

bool HashTable::get(const std::string& key, std::string& value) const {
    Node* node = findNode(key);
    if (node == nullptr) {
        return false;
    }
    value = node->value;
    return true;
}

bool HashTable::remove(const std::string& key, std::string& removed) {
    std::size_t index = bucketIndex(key);
    Node* node = buckets_[index];
    Node* previous = nullptr;
    while (node != nullptr) {
        if (node->key == key) {
            removed = node->value;
            if (previous == nullptr) {
                buckets_[index] = node->next;
            } else {
                previous->next = node->next;
            }
            delete node;
            --size_;
            return true;
        }
        previous = node;
        node = node->next;
    }
    return false;
}

bool HashTable::contains(const std::string& key) const {
    return findNode(key) != nullptr;
}

double HashTable::loadFactor() const noexcept {
    return capacity_ == 0 ? 0.0
                          : static_cast<double>(size_) / static_cast<double>(capacity_);
}

std::size_t HashTable::maxChainLength() const {
    std::size_t maximum = 0;
    for (std::size_t i = 0; i < capacity_; ++i) {
        std::size_t length = 0;
        for (Node* node = buckets_[i]; node != nullptr; node = node->next) {
            ++length;
        }
        maximum = std::max(maximum, length);
    }
    return maximum;
}

void HashTable::clear() noexcept {
    for (std::size_t i = 0; i < capacity_; ++i) {
        Node* node = buckets_[i];
        while (node != nullptr) {
            Node* next = node->next;
            delete node;
            node = next;
        }
        buckets_[i] = nullptr;
    }
    size_ = 0;
    collisions_ = 0;
}

std::string HashTable::toString() const {
    std::ostringstream out;
    out << '{';
    bool first = true;
    for (const auto& entry : entries()) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << entry.first << '=' << entry.second;
    }
    out << '}';
    return out.str();
}

std::vector<std::pair<std::string, std::string>> HashTable::entries() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(size_);
    for (std::size_t i = 0; i < capacity_; ++i) {
        for (Node* node = buckets_[i]; node != nullptr; node = node->next) {
            result.emplace_back(node->key, node->value);
        }
    }
    std::sort(result.begin(), result.end(),
              [](const auto& left, const auto& right) { return left.first < right.first; });
    return result;
}

void HashTable::forEach(
    const std::function<void(const std::string&, const std::string&)>& visitor) const {
    for (std::size_t i = 0; i < capacity_; ++i) {
        for (Node* node = buckets_[i]; node != nullptr; node = node->next) {
            visitor(node->key, node->value);
        }
    }
}

}  // namespace lab3
