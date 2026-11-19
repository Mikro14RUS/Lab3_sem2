#ifndef LAB3_DOUBLY_LINKED_LIST_HPP
#define LAB3_DOUBLY_LINKED_LIST_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace lab3 {

// Двусвязный список строк с обходом в обоих направлениях.
class DoublyLinkedList {
public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    DoublyLinkedList(const DoublyLinkedList& other);
    DoublyLinkedList& operator=(const DoublyLinkedList& other);
    DoublyLinkedList(DoublyLinkedList&& other) noexcept;
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept;

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

    std::string toString(bool reverse = false) const;
    std::vector<std::string> toVector() const;
    void forEach(const std::function<void(const std::string&)>& visitor) const;

private:
    struct Node {
        std::string value;
        Node* next;
        Node* prev;
    };

    Node* nodeAt(std::size_t index) const;
    Node* findNode(const std::string& value) const;
    void unlink(Node* node);

    Node* head_;
    Node* tail_;
    std::size_t size_;
};

}  // namespace lab3

#endif  // LAB3_DOUBLY_LINKED_LIST_HPP
