#ifndef LAB3_QUEUE_HPP
#define LAB3_QUEUE_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace lab3 {

// Очередь FIFO на кольцевом буфере.
class Queue {
public:
    explicit Queue(std::size_t initialCapacity = 8);
    ~Queue();

    Queue(const Queue& other);
    Queue& operator=(const Queue& other);
    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other) noexcept;

    void push(const std::string& value);
    bool pop(std::string& removed);
    bool front(std::string& value) const;

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept { front_ = 0; size_ = 0; }

    std::string toString() const;
    std::vector<std::string> toVector() const;

private:
    void grow();

    std::string* data_;
    std::size_t front_;
    std::size_t size_;
    std::size_t capacity_;
};

}  // namespace lab3

#endif  // LAB3_QUEUE_HPP
