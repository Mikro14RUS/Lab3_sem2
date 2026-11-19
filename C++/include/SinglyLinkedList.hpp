#ifndef LAB3_SINGLY_LINKED_LIST_HPP
#define LAB3_SINGLY_LINKED_LIST_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace lab3 {

// Односвязный список строк.
class SinglyLinkedList {
public:
    SinglyLinkedList();
    ~SinglyLinkedList();

    SinglyLinkedList(const SinglyLinkedList& other);
    SinglyLinkedList& operator=(const SinglyLinkedList& other);
    SinglyLinkedList(SinglyLinkedList&& other) noexcept;
    SinglyLinkedList& operator=(SinglyLinkedList&& other) noexcept;

    void pushFront(const std::string& value);
    void pushBack(const std::string& value);
    bool insertBefore(const std::string& target, const std::string& value);
    bool insertAfter(const std::string& target, const std::string& value);

    bool popFront(std::string& removed);
    bool popBack(std::string& removed);
    bool deleteBefore(const std::string& target, std::string& removed);
    bool deleteAfter(const std::string& target, std::string& removed);
    bool deleteValue(const std::string& value, std::string& removed);

    int find(const std::string& value) const;
    bool getAt(std::size_t index, std::string& value) const;

    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept;

    std::string toString() const;
    std::vector<std::string> toVector() const;
    void forEach(const std::function<void(const std::string&)>& visitor) const;

private:
    struct Node {
        std::string value;
        Node* next;
    };

    Node* nodeAt(std::size_t index) const;
    Node* findNode(const std::string& value) const;

    Node* head_;
    Node* tail_;
    std::size_t size_;
};

}  // namespace lab3

#endif  // LAB3_SINGLY_LINKED_LIST_HPP
