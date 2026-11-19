#include "catch.hpp"

#include <cstdio>
#include <filesystem>
#include <sstream>
#include <string>
#include <utility>
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

namespace {

std::string tempPath(const char* name) {
    return (std::filesystem::temp_directory_path() / name).string();
}

}  // namespace

TEST_CASE("Сериализация Array: запись и чтение") {
    Array source;
    source.pushBack("alpha");
    source.pushBack("beta");
    source.pushBack("gamma");

    std::stringstream stream;
    REQUIRE(serial::saveArray(stream, source));
    Array target;
    target.pushBack("old");
    REQUIRE(serial::loadArray(stream, target));
    REQUIRE(target.toVector() == source.toVector());
}

TEST_CASE("Сериализация односвязного списка") {
    SinglyLinkedList source;
    source.pushBack("first");
    source.pushBack("second");
    source.pushFront("zero");

    std::stringstream stream;
    REQUIRE(serial::saveList(stream, source));
    SinglyLinkedList target;
    REQUIRE(serial::loadList(stream, target));
    REQUIRE(target.toVector() == source.toVector());
    REQUIRE(target.size() == 3);
}

TEST_CASE("Сериализация двусвязного списка") {
    DoublyLinkedList source;
    source.pushBack("b");
    source.pushFront("a");
    source.pushBack("c");

    std::stringstream stream;
    REQUIRE(serial::saveDoubleList(stream, source));
    DoublyLinkedList target;
    REQUIRE(serial::loadDoubleList(stream, target));
    REQUIRE(target.toVector() == source.toVector());
    REQUIRE(target.toString(true) == source.toString(true));
}

TEST_CASE("Сериализация Stack и Queue") {
    Stack stack;
    stack.push("s1");
    stack.push("s2");
    Queue queue;
    queue.push("q1");
    queue.push("q2");

    std::stringstream stackStream;
    std::stringstream queueStream;
    REQUIRE(serial::saveStack(stackStream, stack));
    REQUIRE(serial::saveQueue(queueStream, queue));

    Stack stackCopy;
    Queue queueCopy;
    REQUIRE(serial::loadStack(stackStream, stackCopy));
    REQUIRE(serial::loadQueue(queueStream, queueCopy));
    REQUIRE(stackCopy.toVector() == stack.toVector());
    REQUIRE(queueCopy.toVector() == queue.toVector());
}

TEST_CASE("Сериализация HashTable") {
    HashTable source(4);
    source.put("b", "2");
    source.put("a", "1");
    source.put("c", "3");

    std::stringstream stream;
    REQUIRE(serial::saveHash(stream, source));
    HashTable target;
    target.put("old", "value");
    REQUIRE(serial::loadHash(stream, target));
    REQUIRE(target.entries() == source.entries());
    REQUIRE(target.size() == 3);
}

TEST_CASE("Сериализация FullBinaryTree") {
    FullBinaryTree source;
    for (int value : {50, 30, 70, 20, 40, 60, 80}) {
        REQUIRE(source.insert(value));
    }

    std::stringstream stream;
    REQUIRE(serial::saveTree(stream, source));
    FullBinaryTree target;
    REQUIRE(serial::loadTree(stream, target));
    REQUIRE(target.size() == source.size());
    REQUIRE(target.preorder() == source.preorder());
    REQUIRE(target.isFull());
    REQUIRE(target.isBst());
}

TEST_CASE("Сериализация отвергает неверный тег") {
    std::stringstream stream("BAD!data");
    Array target;
    target.pushBack("saved");
    REQUIRE_FALSE(serial::loadArray(stream, target));
    REQUIRE(target.size() == 1);
}

TEST_CASE("Сериализация отвергает обрезанные данные") {
    Array source;
    source.pushBack("value");
    std::stringstream stream;
    REQUIRE(serial::saveArray(stream, source));
    std::string bytes = stream.str();
    bytes.resize(bytes.size() - 2);

    std::stringstream broken(bytes);
    Array target;
    REQUIRE_FALSE(serial::loadArray(broken, target));
    REQUIRE(target.empty());
}

TEST_CASE("Сериализация дерева отвергает неполный preorder") {
    std::stringstream stream;
    stream.put('T');
    stream.put('R');
    stream.put('E');
    stream.put('1');
    stream.put('\x02');
    stream.put('\x00');
    stream.put('\x00');
    stream.put('\x00');
    stream.put('\x01');
    stream.put('\x05');
    stream.put('\x00');
    stream.put('\x00');
    stream.put('\x00');

    FullBinaryTree target;
    REQUIRE_FALSE(serial::loadTree(stream, target));
    REQUIRE(target.empty());
}

TEST_CASE("Сериализация пустых структур") {
    Array array;
    SinglyLinkedList list;
    DoublyLinkedList doubleList;
    Stack stack;
    Queue queue;
    FullBinaryTree tree;
    HashTable hash;

    std::stringstream stream;
    REQUIRE(serial::saveArray(stream, array));
    REQUIRE(serial::saveList(stream, list));
    REQUIRE(serial::saveDoubleList(stream, doubleList));
    REQUIRE(serial::saveStack(stream, stack));
    REQUIRE(serial::saveQueue(stream, queue));
    REQUIRE(serial::saveHash(stream, hash));
    REQUIRE(serial::saveTree(stream, tree));

    Array arrayCopy;
    SinglyLinkedList listCopy;
    DoublyLinkedList doubleListCopy;
    Stack stackCopy;
    Queue queueCopy;
    FullBinaryTree treeCopy;
    HashTable hashCopy;
    REQUIRE(serial::loadArray(stream, arrayCopy));
    REQUIRE(serial::loadList(stream, listCopy));
    REQUIRE(serial::loadDoubleList(stream, doubleListCopy));
    REQUIRE(serial::loadStack(stream, stackCopy));
    REQUIRE(serial::loadQueue(stream, queueCopy));
    REQUIRE(serial::loadHash(stream, hashCopy));
    REQUIRE(serial::loadTree(stream, treeCopy));
    REQUIRE(arrayCopy.empty());
    REQUIRE(listCopy.empty());
    REQUIRE(doubleListCopy.empty());
    REQUIRE(stackCopy.empty());
    REQUIRE(queueCopy.empty());
    REQUIRE(hashCopy.empty());
    REQUIRE(treeCopy.empty());
}

TEST_CASE("Файловая сериализация всех структур") {
    Array array;
    array.pushBack("alpha");
    array.pushBack("beta");

    SinglyLinkedList list;
    list.pushBack("one");
    list.pushFront("zero");

    DoublyLinkedList doubleList;
    doubleList.pushBack("b");
    doubleList.pushFront("a");
    doubleList.pushBack("c");

    Stack stack;
    stack.push("s1");
    stack.push("s2");

    Queue queue;
    queue.push("q1");
    queue.push("q2");

    HashTable hash(4);
    hash.put("a", "1");
    hash.put("b", "2");

    FullBinaryTree tree;
    for (int value : {50, 30, 70}) {
        REQUIRE(tree.insert(value));
    }

    const std::string arrayPath = tempPath("lab3_array_file.bin");
    const std::string listPath = tempPath("lab3_list_file.bin");
    const std::string doubleListPath = tempPath("lab3_double_list_file.bin");
    const std::string stackPath = tempPath("lab3_stack_file.bin");
    const std::string queuePath = tempPath("lab3_queue_file.bin");
    const std::string hashPath = tempPath("lab3_hash_file.bin");
    const std::string treePath = tempPath("lab3_tree_file.bin");

    REQUIRE(serial::saveArrayToFile(arrayPath, array));
    REQUIRE(serial::saveListToFile(listPath, list));
    REQUIRE(serial::saveDoubleListToFile(doubleListPath, doubleList));
    REQUIRE(serial::saveStackToFile(stackPath, stack));
    REQUIRE(serial::saveQueueToFile(queuePath, queue));
    REQUIRE(serial::saveHashToFile(hashPath, hash));
    REQUIRE(serial::saveTreeToFile(treePath, tree));

    Array arrayCopy;
    SinglyLinkedList listCopy;
    DoublyLinkedList doubleListCopy;
    Stack stackCopy;
    Queue queueCopy;
    HashTable hashCopy;
    FullBinaryTree treeCopy;

    REQUIRE(serial::loadArrayFromFile(arrayPath, arrayCopy));
    REQUIRE(serial::loadListFromFile(listPath, listCopy));
    REQUIRE(serial::loadDoubleListFromFile(doubleListPath, doubleListCopy));
    REQUIRE(serial::loadStackFromFile(stackPath, stackCopy));
    REQUIRE(serial::loadQueueFromFile(queuePath, queueCopy));
    REQUIRE(serial::loadHashFromFile(hashPath, hashCopy));
    REQUIRE(serial::loadTreeFromFile(treePath, treeCopy));

    REQUIRE(arrayCopy.toVector() == array.toVector());
    REQUIRE(listCopy.toVector() == list.toVector());
    REQUIRE(doubleListCopy.toVector() == doubleList.toVector());
    REQUIRE(doubleListCopy.toString(true) == doubleList.toString(true));
    REQUIRE(stackCopy.toVector() == stack.toVector());
    REQUIRE(queueCopy.toVector() == queue.toVector());
    REQUIRE(hashCopy.entries() == hash.entries());
    REQUIRE(treeCopy.size() == tree.size());
    REQUIRE(treeCopy.preorder() == tree.preorder());
    REQUIRE(treeCopy.isFull());
    REQUIRE(treeCopy.isBst());

    Array missing;
    missing.pushBack("saved");
    REQUIRE_FALSE(serial::loadArrayFromFile(tempPath("lab3_missing_file.bin"), missing));
    REQUIRE(missing.toVector() == std::vector<std::string>{"saved"});

    std::remove(arrayPath.c_str());
    std::remove(listPath.c_str());
    std::remove(doubleListPath.c_str());
    std::remove(stackPath.c_str());
    std::remove(queuePath.c_str());
    std::remove(hashPath.c_str());
    std::remove(treePath.c_str());
}
