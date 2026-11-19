# Лабораторная работа №3 — C++

Многофайловая реализация учебных структур данных: динамический массив,
односвязный и двусвязный списки, стек, очередь, хеш-таблица и Full Binary
Tree. Для C++-версии добавлена компактная бинарная сериализация структур;
консольного приложения и базы данных в проекте нет — проверка выполняется
через unit-тесты и бенчмарк.

## Состав

```text
include/        интерфейсы структур и Serialization.hpp
src/            реализации и Serialization.cpp
tests/          общий набор сценариев Catch2 и адаптеры для GoogleTest/Boost
bench/          бенчмарк структур данных
```

## Сборка и проверки

Нужен Catch2 v3 (заголовки и библиотеки `Catch2Main`, `Catch2`). Makefile
сам ищет его в стандартных префиксах. Если Catch2 установлен в другое место,
передайте `CATCH_DIR=/path/to/prefix`.

```bash
make test                 # Catch2, 43 тестовых сценария
make test_gtest           # те же сценарии под Google Test
make test_boost           # те же сценарии под Boost.Test
make test_all             # все три фреймворка
make bench                # бенчмарк, N = 200000
make coverage             # gcovr, порог 85% по строкам и ветвям
make coverage_gtest       # покрытие под Google Test
make coverage_boost       # покрытие под Boost.Test
make lint                 # -Werror и cpplint
```

Системные пакеты для дополнительных фреймворков:

```bash
sudo apt install catch2 libgtest-dev googletest libboost-test-dev
```

Если GoogleTest или Boost установлены в отдельный префикс, используйте
`GTEST_DIR=/path/to/prefix` и `BOOST_DIR=/path/to/prefix`: Makefile ищет
заголовки в `$(GTEST_DIR)/include`, а библиотеки — в
`$(GTEST_DIR)/lib/$(shell g++ -print-multiarch)`.

## Формат бинарной сериализации

Каждая структура записывается отдельно: 4-байтовый тег (`ARR1`, `LST1`,
`DLST`, `STK1`, `QUE1`, `HSH1`, `TRE1`), количество элементов и строки в
формате «длина + байты». Для дерева используется preorder-обход с маркерами
`nullptr`, что позволяет восстановить точную форму Full Binary Tree.

## Файловая сериализация

Для каждой структуры, помимо потоковых `save*`/`load*`, доступны файловые
обёртки:

```cpp
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
```

Все обёртки используют те же бинарные форматы, что и потоковые функции:
`ARR1`, `LST1`, `DLST`, `STK1`, `QUE1`, `HSH1` и `TRE1`. Файл открывается
в режиме `std::ios::binary`; имя файла задаёт вызывающий код. При ошибке
записи частично созданный файл удаляется. Пример:

```cpp
lab3::Array array;
array.pushBack("one");
array.pushBack("two");

lab3::serial::saveArrayToFile("array.bin", array);

lab3::Array restored;
lab3::serial::loadArrayFromFile("array.bin", restored);
```
