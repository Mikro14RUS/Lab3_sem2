#ifndef LAB3_SERIALIZATION_HPP
#define LAB3_SERIALIZATION_HPP

#include <iosfwd>
#include <string>

namespace lab3 {

class Array;
class DoublyLinkedList;
class FullBinaryTree;
class HashTable;
class Queue;
class SinglyLinkedList;
class Stack;

// Бинарная сериализация структур данных. Каждая функция записывает
// одну структуру в поток и проверяет формат при чтении.
namespace serial {

bool saveArray(std::ostream& out, const Array& value);
bool loadArray(std::istream& in, Array& value);

bool saveList(std::ostream& out, const SinglyLinkedList& value);
bool loadList(std::istream& in, SinglyLinkedList& value);

bool saveDoubleList(std::ostream& out, const DoublyLinkedList& value);
bool loadDoubleList(std::istream& in, DoublyLinkedList& value);

bool saveStack(std::ostream& out, const Stack& value);
bool loadStack(std::istream& in, Stack& value);

bool saveQueue(std::ostream& out, const Queue& value);
bool loadQueue(std::istream& in, Queue& value);

bool saveHash(std::ostream& out, const HashTable& value);
bool loadHash(std::istream& in, HashTable& value);

bool saveTree(std::ostream& out, const FullBinaryTree& value);
bool loadTree(std::istream& in, FullBinaryTree& value);

// Файловые обёртки используют те же бинарные форматы, что и потоковые
// функции, но сами открывают файл в бинарном режиме. Имя файла задаёт
// вызывающий код.
bool saveArrayToFile(const std::string& path, const Array& value);
bool loadArrayFromFile(const std::string& path, Array& value);

bool saveListToFile(const std::string& path, const SinglyLinkedList& value);
bool loadListFromFile(const std::string& path, SinglyLinkedList& value);

bool saveDoubleListToFile(const std::string& path, const DoublyLinkedList& value);
bool loadDoubleListFromFile(const std::string& path, DoublyLinkedList& value);

bool saveStackToFile(const std::string& path, const Stack& value);
bool loadStackFromFile(const std::string& path, Stack& value);

bool saveQueueToFile(const std::string& path, const Queue& value);
bool loadQueueFromFile(const std::string& path, Queue& value);

bool saveHashToFile(const std::string& path, const HashTable& value);
bool loadHashFromFile(const std::string& path, HashTable& value);

bool saveTreeToFile(const std::string& path, const FullBinaryTree& value);
bool loadTreeFromFile(const std::string& path, FullBinaryTree& value);

}  // namespace serial
}  // namespace lab3

#endif  // LAB3_SERIALIZATION_HPP
