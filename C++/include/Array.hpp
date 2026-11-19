#ifndef LAB3_ARRAY_HPP
#define LAB3_ARRAY_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace lab3 {

// Динамический массив строк, реализованный без std::vector.
// Класс владеет непрерывным блоком памяти и следует правилу пяти.
class Array {
public:
    explicit Array(std::size_t initialCapacity = 8);
    ~Array();

    Array(const Array& other);
    Array& operator=(const Array& other);
    Array(Array&& other) noexcept;
    Array& operator=(Array&& other) noexcept;

    void pushBack(const std::string& value);
    void pushFront(const std::string& value);
    bool insertAt(std::size_t index, const std::string& value);

    bool popBack(std::string& removed);
    bool popFront(std::string& removed);
    bool removeAt(std::size_t index, std::string& removed);

    bool getAt(std::size_t index, std::string& value) const;
    bool setAt(std::size_t index, const std::string& value);

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept { size_ = 0; }

    std::string toString() const;
    std::vector<std::string> toVector() const;
    void forEach(const std::function<void(const std::string&)>& visitor) const;

private:
    void ensureCapacity(std::size_t needed);

    std::string* data_;
    std::size_t size_;
    std::size_t capacity_;
};

}  // namespace lab3

#endif  // LAB3_ARRAY_HPP
