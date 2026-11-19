#include "Array.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace lab3 {

Array::Array(std::size_t initialCapacity)
    : data_(new std::string[initialCapacity == 0 ? 1 : initialCapacity]),
      size_(0),
      capacity_(initialCapacity == 0 ? 1 : initialCapacity) {}

Array::~Array() {
    delete[] data_;
}

Array::Array(const Array& other)
    : data_(new std::string[other.capacity_ == 0 ? 1 : other.capacity_]),
      size_(other.size_),
      capacity_(other.capacity_ == 0 ? 1 : other.capacity_) {
    for (std::size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
    }
}

Array& Array::operator=(const Array& other) {
    if (this != &other) {
        Array copy(other);
        std::swap(data_, copy.data_);
        std::swap(size_, copy.size_);
        std::swap(capacity_, copy.capacity_);
    }
    return *this;
}

Array::Array(Array&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

Array& Array::operator=(Array&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

void Array::ensureCapacity(std::size_t needed) {
    if (needed <= capacity_) {
        return;
    }
    std::size_t newCapacity = capacity_ == 0 ? 1 : capacity_;
    while (newCapacity < needed) {
        newCapacity *= 2;
    }
    std::string* newData = new std::string[newCapacity];
    for (std::size_t i = 0; i < size_; ++i) {
        newData[i] = std::move(data_[i]);
    }
    delete[] data_;
    data_ = newData;
    capacity_ = newCapacity;
}

void Array::pushBack(const std::string& value) {
    ensureCapacity(size_ + 1);
    data_[size_++] = value;
}

void Array::pushFront(const std::string& value) {
    insertAt(0, value);
}

bool Array::insertAt(std::size_t index, const std::string& value) {
    if (index > size_) {
        return false;
    }
    ensureCapacity(size_ + 1);
    for (std::size_t i = size_; i > index; --i) {
        data_[i] = std::move(data_[i - 1]);
    }
    data_[index] = value;
    ++size_;
    return true;
}

bool Array::popBack(std::string& removed) {
    if (size_ == 0) {
        return false;
    }
    removed = data_[--size_];
    return true;
}

bool Array::popFront(std::string& removed) {
    return removeAt(0, removed);
}

bool Array::removeAt(std::size_t index, std::string& removed) {
    if (index >= size_) {
        return false;
    }
    removed = data_[index];
    for (std::size_t i = index; i + 1 < size_; ++i) {
        data_[i] = std::move(data_[i + 1]);
    }
    --size_;
    return true;
}

bool Array::getAt(std::size_t index, std::string& value) const {
    if (index >= size_) {
        return false;
    }
    value = data_[index];
    return true;
}

bool Array::setAt(std::size_t index, const std::string& value) {
    if (index >= size_) {
        return false;
    }
    data_[index] = value;
    return true;
}

std::string Array::toString() const {
    std::ostringstream out;
    out << '[';
    for (std::size_t i = 0; i < size_; ++i) {
        if (i != 0) {
            out << ", ";
        }
        out << data_[i];
    }
    out << ']';
    return out.str();
}

std::vector<std::string> Array::toVector() const {
    return std::vector<std::string>(data_, data_ + size_);
}

void Array::forEach(const std::function<void(const std::string&)>& visitor) const {
    for (std::size_t i = 0; i < size_; ++i) {
        visitor(data_[i]);
    }
}

}  // namespace lab3
