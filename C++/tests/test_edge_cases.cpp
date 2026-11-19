#include "catch.hpp"

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "Array.hpp"
#include "DoublyLinkedList.hpp"
#include "FullBinaryTree.hpp"
#include "HashTable.hpp"
#include "Queue.hpp"
#include "Serialization.hpp"
#include "SinglyLinkedList.hpp"
#include "Stack.hpp"

using namespace lab3;

TEST_CASE("Array: нулевая ёмкость и перемещённый объект") {
    Array zero(0);
    REQUIRE(zero.capacity() == 1);
    zero.pushBack("x");
    REQUIRE(zero.toString() == "[x]");

    Array source(1);
    source.pushBack("moved");
    Array moved(std::move(source));
    Array copy(source);
    REQUIRE(copy.empty());
    source.pushBack("again");
    REQUIRE(source.toString() == "[again]");

    Array self;
    self = self;
    REQUIRE(self.empty());
}

TEST_CASE("SinglyLinkedList: пустой список и удаление в середине") {
    SinglyLinkedList list;
    std::string removed;
    REQUIRE(list.find("missing") == -1);
    REQUIRE_FALSE(list.getAt(0, removed));
    REQUIRE_FALSE(list.insertBefore("x", "y"));
    REQUIRE_FALSE(list.insertAfter("x", "y"));

    list.pushBack("a");
    list.pushBack("b");
    list.pushBack("c");
    list.pushBack("d");
    REQUIRE_FALSE(list.deleteAfter("d", removed));
    REQUIRE(list.deleteBefore("d", removed));
    REQUIRE(removed == "c");
    REQUIRE(list.toString() == "a -> b -> d -> NULL");

    REQUIRE(list.deleteAfter("b", removed));
    REQUIRE(removed == "d");
    REQUIRE(list.toString() == "a -> b -> NULL");

    REQUIRE(list.deleteValue("b", removed));
    REQUIRE(removed == "b");
    REQUIRE(list.deleteValue("a", removed));
    REQUIRE(removed == "a");
    REQUIRE(list.empty());
}

TEST_CASE("DoublyLinkedList: пустой список и удаление в середине") {
    DoublyLinkedList list;
    std::string removed;
    REQUIRE(list.find("missing") == -1);
    REQUIRE_FALSE(list.getAt(0, removed));
    REQUIRE_FALSE(list.insertBefore("x", "y"));
    REQUIRE_FALSE(list.insertAfter("x", "y"));
    REQUIRE(list.toString(true) == "NULL");

    list.pushBack("a");
    list.pushBack("b");
    list.pushBack("c");
    list.pushBack("d");
    REQUIRE_FALSE(list.deleteAfter("d", removed));
    REQUIRE(list.deleteBefore("d", removed));
    REQUIRE(removed == "c");
    REQUIRE(list.toString() == "a <-> b <-> d <-> NULL");

    REQUIRE(list.deleteAfter("a", removed));
    REQUIRE(removed == "b");
    REQUIRE(list.deleteValue("d", removed));
    REQUIRE(removed == "d");
    REQUIRE(list.deleteValue("a", removed));
    REQUIRE(removed == "a");
    REQUIRE(list.empty());
}

TEST_CASE("Queue: нулевая ёмкость и перемещение") {
    Queue zero(0);
    REQUIRE(zero.capacity() == 1);
    zero.push("x");
    REQUIRE(zero.size() == 1);
    std::string removed;
    REQUIRE(zero.pop(removed));

    Queue source(1);
    source.push("a");
    Queue moved(std::move(source));
    source.push("after-move");
    REQUIRE(source.size() == 1);
    REQUIRE(moved.size() == 1);
}

TEST_CASE("HashTable: пустая таблица и удаление в середине") {
    HashTable table;
    REQUIRE(table.empty());
    REQUIRE(table.maxChainLength() == 0);
    REQUIRE(table.loadFactor() == 0.0);
    REQUIRE(table.entries().empty());
    REQUIRE_FALSE(table.contains("missing"));

    HashTable zero(0);
    REQUIRE(zero.bucketCount() >= 1);

    table.put("a", "1");
    table.put("b", "2");
    table.put("c", "3");
    std::string removed;
    REQUIRE(table.remove("b", removed));
    REQUIRE(removed == "2");
    REQUIRE(table.remove("c", removed));
    REQUIRE(removed == "3");
    REQUIRE(table.remove("a", removed));
    REQUIRE(removed == "1");
    REQUIRE(table.empty());
}

TEST_CASE("FullBinaryTree: обход Pretty с одним потомком") {
    FullBinaryTree right;
    right.insert(10);
    right.insert(20);
    right.insert(30);
    REQUIRE(right.toString(FullBinaryTree::Traversal::Pretty).find("10") !=
            std::string::npos);

    FullBinaryTree left;
    left.insert(10);
    left.insert(5);
    left.insert(1);
    REQUIRE(left.toString(FullBinaryTree::Traversal::Pretty).find("10") !=
            std::string::npos);

    FullBinaryTree empty;
    REQUIRE(empty.preorder().empty());
    REQUIRE(empty.inorder().empty());
    REQUIRE(empty.postorder().empty());
}

TEST_CASE("Сериализация: неверные теги всех форматов") {
    std::stringstream stream("BAD!");
    Array array;
    SinglyLinkedList list;
    DoublyLinkedList doubleList;
    Stack stack;
    Queue queue;
    HashTable hash;
    FullBinaryTree tree;
    REQUIRE_FALSE(serial::loadArray(stream, array));
    REQUIRE_FALSE(serial::loadList(stream, list));
    REQUIRE_FALSE(serial::loadDoubleList(stream, doubleList));
    REQUIRE_FALSE(serial::loadStack(stream, stack));
    REQUIRE_FALSE(serial::loadQueue(stream, queue));
    REQUIRE_FALSE(serial::loadHash(stream, hash));
    REQUIRE_FALSE(serial::loadTree(stream, tree));
}

TEST_CASE("Сериализация: ошибки записи в поток") {
    std::ostringstream broken;
    broken.setstate(std::ios::badbit);
    Array array;
    array.pushBack("x");
    SinglyLinkedList list;
    list.pushBack("x");
    DoublyLinkedList doubleList;
    doubleList.pushBack("x");
    Stack stack;
    stack.push("x");
    Queue queue;
    queue.push("x");
    HashTable hash;
    hash.put("k", "v");
    FullBinaryTree tree;
    tree.insert(1);

    REQUIRE_FALSE(serial::saveArray(broken, array));
    REQUIRE_FALSE(serial::saveList(broken, list));
    REQUIRE_FALSE(serial::saveDoubleList(broken, doubleList));
    REQUIRE_FALSE(serial::saveStack(broken, stack));
    REQUIRE_FALSE(serial::saveQueue(broken, queue));
    REQUIRE_FALSE(serial::saveHash(broken, hash));
    REQUIRE_FALSE(serial::saveTree(broken, tree));
}

TEST_CASE("Сериализация: пустая строка и слишком большой размер") {
    Array emptyValue;
    emptyValue.pushBack("");
    std::stringstream stream;
    REQUIRE(serial::saveArray(stream, emptyValue));
    Array restored;
    REQUIRE(serial::loadArray(stream, restored));
    REQUIRE(restored.size() == 1);
    std::string value;
    REQUIRE(restored.getAt(0, value));
    REQUIRE(value.empty());

    std::stringstream tooLong;
    tooLong.write("ARR1", 4);
    const std::uint8_t count[4] = {1, 0, 0, 0};
    tooLong.write(reinterpret_cast<const char*>(count), 4);
    const std::uint8_t length[4] = {1, 0, 0, 1};  // 16 МиБ + 1
    tooLong.write(reinterpret_cast<const char*>(length), 4);
    REQUIRE_FALSE(serial::loadArray(tooLong, restored));
}

TEST_CASE("Сериализация: обрезанные последовательности и таблица") {
    SinglyLinkedList list;
    list.pushBack("a");
    list.pushBack("b");
    std::stringstream listStream;
    REQUIRE(serial::saveList(listStream, list));
    std::string bytes = listStream.str();
    bytes.resize(bytes.size() - 1);
    std::stringstream brokenList(bytes);
    SinglyLinkedList listCopy;
    REQUIRE_FALSE(serial::loadList(brokenList, listCopy));

    HashTable table;
    table.put("a", "1");
    std::stringstream hashStream;
    REQUIRE(serial::saveHash(hashStream, table));
    bytes = hashStream.str();
    bytes.resize(bytes.size() - 1);
    std::stringstream brokenHash(bytes);
    HashTable hashCopy;
    REQUIRE_FALSE(serial::loadHash(brokenHash, hashCopy));
}

TEST_CASE("Сериализация: некорректные маркеры дерева") {
    std::stringstream badMarker;
    badMarker.write("TRE1", 4);
    const std::uint8_t count[4] = {1, 0, 0, 0};
    badMarker.write(reinterpret_cast<const char*>(count), 4);
    const std::uint8_t marker = 2;
    badMarker.write(reinterpret_cast<const char*>(&marker), 1);
    FullBinaryTree tree;
    REQUIRE_FALSE(serial::loadTree(badMarker, tree));

    std::stringstream tooLarge;
    tooLarge.write("TRE1", 4);
    const std::uint8_t huge[4] = {1, 0, 0, 1};
    tooLarge.write(reinterpret_cast<const char*>(huge), 4);
    REQUIRE_FALSE(serial::loadTree(tooLarge, tree));
}
