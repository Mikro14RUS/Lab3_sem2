#ifndef LAB3_STACK_HPP
#define LAB3_STACK_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "Array.hpp"

namespace lab3 {

// Стек LIFO на основе собственного динамического массива (композиция).
class Stack {
public:
    Stack() = default;
    explicit Stack(std::size_t initialCapacity) : data_(initialCapacity) {}

    void push(const std::string& value) { data_.pushBack(value); }
    bool pop(std::string& removed) { return data_.popBack(removed); }
    bool peek(std::string& value) const;

    std::size_t size() const noexcept { return data_.size(); }
    bool empty() const noexcept { return data_.empty(); }
    void clear() noexcept { data_.clear(); }

    std::string toString() const;
    std::vector<std::string> toVector() const { return data_.toVector(); }

private:
    Array data_;
};

}  // namespace lab3

#endif  // LAB3_STACK_HPP
