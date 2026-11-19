#include "catch.hpp"

#include <string>

#include "Queue.hpp"
#include "Stack.hpp"

using lab3::Queue;
using lab3::Stack;

TEST_CASE("Stack: LIFO и граничные случаи") {
    Stack stack(1);
    REQUIRE(stack.empty());
    REQUIRE(stack.size() == 0);
    REQUIRE(stack.toString() == "Стек пуст");

    std::string value;
    REQUIRE_FALSE(stack.peek(value));
    REQUIRE_FALSE(stack.pop(value));

    stack.push("one");
    stack.push("two");
    stack.push("three");
    REQUIRE(stack.size() == 3);
    REQUIRE(stack.toString() == "вершина -> three two one <- дно");
    REQUIRE(stack.peek(value));
    REQUIRE(value == "three");

    REQUIRE(stack.pop(value));
    REQUIRE(value == "three");
    REQUIRE(stack.size() == 2);
    REQUIRE(stack.pop(value));
    REQUIRE(value == "two");
    REQUIRE(stack.pop(value));
    REQUIRE(value == "one");
    REQUIRE_FALSE(stack.pop(value));

    stack.push("x");
    stack.clear();
    REQUIRE(stack.empty());
}

TEST_CASE("Stack: копирование и перемещение") {
    Stack source;
    source.push("a");
    source.push("b");

    Stack copy(source);
    REQUIRE(copy.size() == 2);
    copy.push("c");
    REQUIRE(source.size() == 2);

    Stack assigned;
    assigned = source;
    REQUIRE(assigned.size() == 2);

    Stack moved(std::move(copy));
    REQUIRE(moved.size() == 3);
    REQUIRE(copy.empty());
    Stack moveAssigned;
    moveAssigned = std::move(moved);
    REQUIRE(moveAssigned.size() == 3);
    REQUIRE(moved.empty());
}

TEST_CASE("Queue: FIFO, кольцевой буфер и рост") {
    Queue queue(2);
    REQUIRE(queue.empty());
    REQUIRE(queue.capacity() == 2);
    REQUIRE(queue.toString() == "Очередь пуста");

    std::string value;
    REQUIRE_FALSE(queue.pop(value));
    REQUIRE_FALSE(queue.front(value));

    queue.push("a");
    queue.push("b");
    REQUIRE(queue.size() == 2);
    REQUIRE(queue.toString() == "начало -> a b <- конец");
    REQUIRE(queue.front(value));
    REQUIRE(value == "a");

    REQUIRE(queue.pop(value));
    REQUIRE(value == "a");
    queue.push("c");
    queue.push("d");
    REQUIRE(queue.size() == 3);
    REQUIRE(queue.toString() == "начало -> b c d <- конец");
    REQUIRE(queue.toVector().size() == 3);
    REQUIRE(queue.toVector()[0] == "b");
    REQUIRE(queue.toVector()[2] == "d");

    queue.clear();
    REQUIRE(queue.empty());
}

TEST_CASE("Queue: копирование и перемещение") {
    Queue source(2);
    source.push("a");
    source.push("b");
    source.push("c");

    Queue copy(source);
    REQUIRE(copy.size() == 3);
    REQUIRE(copy.toString() == "начало -> a b c <- конец");
    copy.push("d");
    REQUIRE(source.size() == 3);

    Queue assigned;
    assigned = source;
    REQUIRE(assigned.size() == 3);
    assigned = assigned;

    Queue moved(std::move(copy));
    REQUIRE(moved.size() == 4);
    REQUIRE(copy.empty());
    Queue moveAssigned;
    moveAssigned = std::move(moved);
    REQUIRE(moveAssigned.size() == 4);
    REQUIRE(moved.empty());
}
