#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Array.hpp"
#include "DoublyLinkedList.hpp"
#include "FullBinaryTree.hpp"
#include "HashTable.hpp"
#include "Queue.hpp"
#include "SinglyLinkedList.hpp"
#include "Stack.hpp"

namespace {

using Clock = std::chrono::steady_clock;

double milliseconds(Clock::time_point start, Clock::time_point finish) {
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

void line(const std::string& name, double ms, std::size_t operations,
          const std::string& note = "") {
    const double perSecond = ms > 0.0 ? static_cast<double>(operations) / ms * 1000.0 : 0.0;
    std::cout << std::left << std::setw(30) << name << std::right << std::setw(10)
              << std::fixed << std::setprecision(3) << ms << " мс" << std::setw(14)
              << static_cast<long long>(perSecond) << " оп/с";
    if (!note.empty()) {
        std::cout << "  " << note;
    }
    std::cout << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    std::size_t n = 200000;
    if (argc > 1) {
        try {
            n = static_cast<std::size_t>(std::stoull(argv[1]));
        } catch (...) {
            std::cerr << "Ошибка: N должно быть целым числом\n";
            return 1;
        }
    }
    if (n == 0) {
        n = 1;
    }

    std::cout << "=== Бенчмарк структур данных, N = " << n << " ===\n";
    std::cout << "Компилятор: g++ -std=c++17 -O2, Linux x86_64\n\n";

    {
        lab3::Array array;
        const auto start = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            array.pushBack("value_" + std::to_string(i));
        }
        const auto finish = Clock::now();
        line("Array pushBack", milliseconds(start, finish), n);

        volatile std::size_t checksum = 0;
        const auto getStart = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            std::string value;
            if (array.getAt(i, value)) {
                checksum += value.size();
            }
        }
        line("Array getAt", milliseconds(getStart, Clock::now()), n);

        const auto popStart = Clock::now();
        std::string removed;
        for (std::size_t i = 0; i < n; ++i) {
            array.popBack(removed);
        }
        line("Array popBack", milliseconds(popStart, Clock::now()), n);
    }

    {
        lab3::SinglyLinkedList list;
        const auto start = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            list.pushBack("value_" + std::to_string(i));
        }
        line("SinglyList pushBack", milliseconds(start, Clock::now()), n);

        const auto frontStart = Clock::now();
        std::string removed;
        for (std::size_t i = 0; i < n; ++i) {
            list.popFront(removed);
        }
        line("SinglyList popFront", milliseconds(frontStart, Clock::now()), n);
    }

    {
        lab3::DoublyLinkedList list;
        const auto start = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            list.pushBack("value_" + std::to_string(i));
        }
        const auto finish = Clock::now();
        line("DoubleList pushBack", milliseconds(start, finish), n);

        const auto popStart = Clock::now();
        std::string removed;
        for (std::size_t i = 0; i < n; ++i) {
            list.popBack(removed);
        }
        line("DoubleList popBack", milliseconds(popStart, Clock::now()), n);
    }

    {
        lab3::Stack stack;
        const auto start = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            stack.push("value_" + std::to_string(i));
        }
        line("Stack push", milliseconds(start, Clock::now()), n);

        const auto popStart = Clock::now();
        std::string removed;
        for (std::size_t i = 0; i < n; ++i) {
            stack.pop(removed);
        }
        line("Stack pop", milliseconds(popStart, Clock::now()), n);
    }

    {
        lab3::Queue queue;
        const auto start = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            queue.push("value_" + std::to_string(i));
        }
        line("Queue push", milliseconds(start, Clock::now()), n);

        const auto popStart = Clock::now();
        std::string removed;
        for (std::size_t i = 0; i < n; ++i) {
            queue.pop(removed);
        }
        line("Queue pop", milliseconds(popStart, Clock::now()), n);
    }

    {
        lab3::HashTable table;
        std::vector<std::string> keys;
        keys.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            keys.push_back("key_" + std::to_string(i));
        }

        const auto start = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            table.put(keys[i], std::to_string(i));
        }
        const auto finish = Clock::now();
        line("HashTable put", milliseconds(start, finish), n,
             "load=" + std::to_string(table.loadFactor()));

        volatile std::size_t checksum = 0;
        const auto getStart = Clock::now();
        for (std::size_t i = 0; i < n; ++i) {
            std::string value;
            if (table.get(keys[i], value)) {
                checksum += value.size();
            }
        }
        line("HashTable get", milliseconds(getStart, Clock::now()), n);
    }

    {
        lab3::FullBinaryTree tree;
        std::mt19937 generator(20261108U);
        std::uniform_int_distribution<int> distribution(-1000000000, 1000000000);
        std::vector<int> values;
        values.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            values.push_back(distribution(generator));
        }

        const auto start = Clock::now();
        for (int value : values) {
            tree.insert(value);
        }
        const auto finish = Clock::now();
        line("FullBinaryTree insert", milliseconds(start, finish), tree.size(),
             "size=" + std::to_string(tree.size()));

        volatile std::size_t found = 0;
        const auto findStart = Clock::now();
        for (int value : values) {
            if (tree.find(value)) {
                ++found;
            }
        }
        line("FullBinaryTree find", milliseconds(findStart, Clock::now()), n);
    }

    std::cout << "\nБенчмарк завершён.\n";
    return 0;
}
