#include "catch.hpp"

#include <string>
#include <vector>

#include "Array.hpp"

using lab3::Array;

TEST_CASE("Array: базовые операции") {
    Array array(1);
    REQUIRE(array.empty());
    REQUIRE(array.size() == 0);
    REQUIRE(array.toString() == "[]");

    array.pushBack("one");
    array.pushBack("two");
    array.pushFront("zero");
    REQUIRE(array.size() == 3);
    REQUIRE(array.toString() == "[zero, one, two]");
    REQUIRE(array.toVector() == std::vector<std::string>{"zero", "one", "two"});

    std::string value;
    REQUIRE(array.getAt(1, value));
    REQUIRE(value == "one");
    REQUIRE(array.setAt(1, "ONE"));
    REQUIRE(array.getAt(1, value));
    REQUIRE(value == "ONE");

    REQUIRE(array.insertAt(1, "middle"));
    REQUIRE(array.toString() == "[zero, middle, ONE, two]");
    REQUIRE_FALSE(array.insertAt(100, "bad"));
    REQUIRE_FALSE(array.getAt(100, value));
    REQUIRE_FALSE(array.setAt(100, "bad"));
}

TEST_CASE("Array: удаление и очистка") {
    Array array;
    array.pushBack("a");
    array.pushBack("b");
    array.pushBack("c");

    std::string removed;
    REQUIRE(array.popBack(removed));
    REQUIRE(removed == "c");
    REQUIRE(array.popFront(removed));
    REQUIRE(removed == "a");
    REQUIRE(array.removeAt(0, removed));
    REQUIRE(removed == "b");
    REQUIRE(array.empty());
    REQUIRE_FALSE(array.popBack(removed));
    REQUIRE_FALSE(array.popFront(removed));
    REQUIRE_FALSE(array.removeAt(0, removed));

    array.pushBack("x");
    array.clear();
    REQUIRE(array.empty());
}

TEST_CASE("Array: копирование, перемещение и forEach") {
    Array source;
    source.pushBack("a");
    source.pushBack("b");

    Array copy(source);
    REQUIRE(copy.toString() == "[a, b]");
    copy.pushBack("c");
    REQUIRE(source.size() == 2);

    Array assigned;
    assigned = source;
    REQUIRE(assigned.toString() == "[a, b]");
    assigned = assigned;
    REQUIRE(assigned.toString() == "[a, b]");

    Array moved(std::move(copy));
    REQUIRE(moved.toString() == "[a, b, c]");
    REQUIRE(copy.empty());

    Array moveAssigned;
    moveAssigned = std::move(moved);
    REQUIRE(moveAssigned.toString() == "[a, b, c]");
    REQUIRE(moved.empty());

    std::string joined;
    source.forEach([&joined](const std::string& item) { joined += item; });
    REQUIRE(joined == "ab");
}
