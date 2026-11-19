#ifndef LAB3_HASH_TABLE_HPP
#define LAB3_HASH_TABLE_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace lab3 {

// Хеш-таблица с разрешением коллизий методом цепочек.
// Ключи и значения — строки. Таблица автоматически перестраивается
// при коэффициенте заполнения выше 0,75.
class HashTable {
public:
    explicit HashTable(std::size_t initialCapacity = 16);
    ~HashTable();

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    // Возвращает true, если добавлен новый ключ, false — если значение обновлено.
    bool put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value) const;
    bool remove(const std::string& key, std::string& removed);
    bool contains(const std::string& key) const;

    std::size_t size() const noexcept { return size_; }
    std::size_t bucketCount() const noexcept { return capacity_; }
    double loadFactor() const noexcept;
    std::size_t collisions() const noexcept { return collisions_; }
    std::size_t maxChainLength() const;
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept;

    std::string toString() const;
    std::vector<std::pair<std::string, std::string>> entries() const;
    void forEach(const std::function<void(const std::string&, const std::string&)>& visitor) const;

    static std::size_t hashOf(const std::string& key);

private:
    struct Node {
        std::string key;
        std::string value;
        Node* next;
    };

    std::size_t bucketIndex(const std::string& key) const;
    Node* findNode(const std::string& key) const;
    void rehash(std::size_t newCapacity);

    Node** buckets_;
    std::size_t capacity_;
    std::size_t size_;
    std::size_t collisions_;
};

}  // namespace lab3

#endif  // LAB3_HASH_TABLE_HPP
