#include "Queue.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace lab3 {

Queue::Queue(std::size_t initialCapacity)
    : data_(new std::string[initialCapacity == 0 ? 1 : initialCapacity]),
      front_(0),
      size_(0),
      capacity_(initialCapacity == 0 ? 1 : initialCapacity) {}

Queue::~Queue() {
    delete[] data_;
}

Queue::Queue(const Queue& other)
    : data_(new std::string[other.capacity_ == 0 ? 1 : other.capacity_]),
      front_(other.front_),
      size_(other.size_),
      capacity_(other.capacity_ == 0 ? 1 : other.capacity_) {
    for (std::size_t i = 0; i < capacity_; ++i) {
        data_[i] = other.data_[i];
    }
}

Queue& Queue::operator=(const Queue& other) {
    if (this != &other) {
        Queue copy(other);
        std::swap(data_, copy.data_);
        std::swap(front_, copy.front_);
        std::swap(size_, copy.size_);
        std::swap(capacity_, copy.capacity_);
    }
    return *this;
}

Queue::Queue(Queue&& other) noexcept
    : data_(other.data_),
      front_(other.front_),
      size_(other.size_),
      capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.front_ = 0;
    other.size_ = 0;
    other.capacity_ = 0;
}

Queue& Queue::operator=(Queue&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        front_ = other.front_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.front_ = 0;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

void Queue::grow() {
    std::size_t newCapacity = capacity_ == 0 ? 1 : capacity_ * 2;
    std::string* newData = new std::string[newCapacity];
    for (std::size_t i = 0; i < size_; ++i) {
        newData[i] = std::move(data_[(front_ + i) % capacity_]);
    }
    delete[] data_;
    data_ = newData;
    capacity_ = newCapacity;
    front_ = 0;
}

void Queue::push(const std::string& value) {
    if (size_ == capacity_) {
        grow();
    }
    data_[(front_ + size_) % capacity_] = value;
    ++size_;
}

bool Queue::pop(std::string& removed) {
    if (size_ == 0) {
        return false;
    }
    removed = data_[front_];
    front_ = (front_ + 1) % capacity_;
    --size_;
    return true;
}

bool Queue::front(std::string& value) const {
    if (size_ == 0) {
        return false;
    }
    value = data_[front_];
    return true;
}

std::string Queue::toString() const {
    if (size_ == 0) {
        return "Очередь пуста";
    }
    std::ostringstream out;
    out << "начало -> ";
    for (std::size_t i = 0; i < size_; ++i) {
        out << data_[(front_ + i) % capacity_];
        if (i + 1 != size_) {
            out << ' ';
        }
    }
    out << " <- конец";
    return out.str();
}

std::vector<std::string> Queue::toVector() const {
    std::vector<std::string> values;
    values.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        values.push_back(data_[(front_ + i) % capacity_]);
    }
    return values;
}

}  // namespace lab3
