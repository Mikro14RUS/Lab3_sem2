#include "catch.hpp"

#include <string>
#include <vector>

#include "DoublyLinkedList.hpp"
#include "SinglyLinkedList.hpp"

using lab3::DoublyLinkedList;
using lab3::SinglyLinkedList;

TEST_CASE("SinglyLinkedList: добавление и чтение") {
    SinglyLinkedList list;
    REQUIRE(list.empty());
    REQUIRE(list.toString() == "NULL");

    list.pushFront("b");
    list.pushFront("a");
    list.pushBack("c");
    REQUIRE(list.size() == 3);
    REQUIRE(list.toString() == "a -> b -> c -> NULL");
    REQUIRE(list.toVector() == std::vector<std::string>{"a", "b", "c"});
    REQUIRE(list.find("b") == 1);
    REQUIRE(list.find("z") == -1);

    std::string value;
    REQUIRE(list.getAt(2, value));
    REQUIRE(value == "c");
    REQUIRE_FALSE(list.getAt(5, value));

    REQUIRE(list.insertBefore("b", "ab"));
    REQUIRE(list.insertAfter("b", "bc"));
    REQUIRE(list.toString() == "a -> ab -> b -> bc -> c -> NULL");
    REQUIRE(list.insertBefore("a", "head"));
    REQUIRE(list.insertAfter("c", "tail"));
    REQUIRE(list.toString() == "head -> a -> ab -> b -> bc -> c -> tail -> NULL");
    REQUIRE_FALSE(list.insertBefore("missing", "x"));
    REQUIRE_FALSE(list.insertAfter("missing", "x"));
}

TEST_CASE("SinglyLinkedList: удаление") {
    SinglyLinkedList list;
    std::string removed;
    REQUIRE_FALSE(list.popFront(removed));
    REQUIRE_FALSE(list.popBack(removed));
    REQUIRE_FALSE(list.deleteBefore("a", removed));
    REQUIRE_FALSE(list.deleteAfter("a", removed));
    REQUIRE_FALSE(list.deleteValue("a", removed));

    list.pushBack("a");
    list.pushBack("b");
    list.pushBack("c");
    list.pushBack("d");

    REQUIRE_FALSE(list.deleteBefore("a", removed));
    REQUIRE(list.deleteBefore("b", removed));
    REQUIRE(removed == "a");
    REQUIRE(list.toString() == "b -> c -> d -> NULL");

    REQUIRE(list.deleteAfter("b", removed));
    REQUIRE(removed == "c");
    REQUIRE(list.toString() == "b -> d -> NULL");

    REQUIRE(list.deleteValue("d", removed));
    REQUIRE(removed == "d");
    REQUIRE(list.toString() == "b -> NULL");

    REQUIRE(list.popFront(removed));
    REQUIRE(removed == "b");
    REQUIRE(list.empty());

    list.pushBack("x");
    list.pushBack("y");
    REQUIRE(list.popBack(removed));
    REQUIRE(removed == "y");
    REQUIRE(list.popBack(removed));
    REQUIRE(removed == "x");
    REQUIRE_FALSE(list.deleteBefore("missing", removed));
    REQUIRE_FALSE(list.deleteAfter("missing", removed));
    REQUIRE_FALSE(list.deleteValue("missing", removed));

    list.pushBack("one");
    list.pushBack("two");
    list.clear();
    REQUIRE(list.empty());
}

TEST_CASE("SinglyLinkedList: копирование и перемещение") {
    SinglyLinkedList source;
    source.pushBack("a");
    source.pushBack("b");

    SinglyLinkedList copy(source);
    REQUIRE(copy.toString() == "a -> b -> NULL");
    copy.pushBack("c");
    REQUIRE(source.size() == 2);

    SinglyLinkedList assigned;
    assigned = source;
    REQUIRE(assigned.toString() == "a -> b -> NULL");
    assigned = assigned;

    SinglyLinkedList moved(std::move(copy));
    REQUIRE(moved.toString() == "a -> b -> c -> NULL");
    REQUIRE(copy.empty());

    SinglyLinkedList moveAssigned;
    moveAssigned = std::move(moved);
    REQUIRE(moveAssigned.size() == 3);
    REQUIRE(moved.empty());

    std::string joined;
    source.forEach([&joined](const std::string& item) { joined += item; });
    REQUIRE(joined == "ab");
}

TEST_CASE("DoublyLinkedList: добавление, чтение и обход") {
    DoublyLinkedList list;
    REQUIRE(list.empty());
    REQUIRE(list.toString() == "NULL");

    list.pushFront("b");
    list.pushFront("a");
    list.pushBack("c");
    REQUIRE(list.toString() == "a <-> b <-> c <-> NULL");
    REQUIRE(list.toString(true) == "c <-> b <-> a <-> NULL");
    REQUIRE(list.toVector() == std::vector<std::string>{"a", "b", "c"});
    REQUIRE(list.find("b") == 1);
    REQUIRE(list.find("z") == -1);

    std::string value;
    REQUIRE(list.getAt(0, value));
    REQUIRE(value == "a");
    REQUIRE_FALSE(list.getAt(9, value));

    REQUIRE(list.insertBefore("b", "ab"));
    REQUIRE(list.insertAfter("b", "bc"));
    REQUIRE(list.toString() == "a <-> ab <-> b <-> bc <-> c <-> NULL");
    REQUIRE(list.insertBefore("a", "head"));
    REQUIRE(list.insertAfter("c", "tail"));
    REQUIRE(list.toString() ==
            "head <-> a <-> ab <-> b <-> bc <-> c <-> tail <-> NULL");
    REQUIRE_FALSE(list.insertBefore("missing", "x"));
    REQUIRE_FALSE(list.insertAfter("missing", "x"));
}

TEST_CASE("DoublyLinkedList: удаление") {
    DoublyLinkedList list;
    std::string removed;
    REQUIRE_FALSE(list.popFront(removed));
    REQUIRE_FALSE(list.popBack(removed));
    REQUIRE_FALSE(list.deleteBefore("a", removed));
    REQUIRE_FALSE(list.deleteAfter("a", removed));
    REQUIRE_FALSE(list.deleteValue("a", removed));

    list.pushBack("a");
    list.pushBack("b");
    list.pushBack("c");
    list.pushBack("d");

    REQUIRE_FALSE(list.deleteBefore("a", removed));
    REQUIRE(list.deleteBefore("b", removed));
    REQUIRE(removed == "a");
    REQUIRE(list.toString() == "b <-> c <-> d <-> NULL");

    REQUIRE(list.deleteAfter("b", removed));
    REQUIRE(removed == "c");
    REQUIRE(list.deleteValue("d", removed));
    REQUIRE(removed == "d");
    REQUIRE(list.popFront(removed));
    REQUIRE(removed == "b");
    REQUIRE(list.empty());

    list.pushBack("x");
    list.pushBack("y");
    REQUIRE(list.popBack(removed));
    REQUIRE(removed == "y");
    REQUIRE(list.popBack(removed));
    REQUIRE(removed == "x");
    REQUIRE_FALSE(list.deleteBefore("missing", removed));
    REQUIRE_FALSE(list.deleteAfter("missing", removed));
    REQUIRE_FALSE(list.deleteValue("missing", removed));

    list.pushBack("one");
    list.clear();
    REQUIRE(list.empty());
}

TEST_CASE("DoublyLinkedList: копирование, перемещение и forEach") {
    DoublyLinkedList source;
    source.pushBack("a");
    source.pushBack("b");

    DoublyLinkedList copy(source);
    REQUIRE(copy.toString() == "a <-> b <-> NULL");
    copy.pushBack("c");
    REQUIRE(source.size() == 2);

    DoublyLinkedList assigned;
    assigned = source;
    REQUIRE(assigned.toString() == "a <-> b <-> NULL");
    assigned = assigned;

    DoublyLinkedList moved(std::move(copy));
    REQUIRE(moved.size() == 3);
    REQUIRE(copy.empty());

    DoublyLinkedList moveAssigned;
    moveAssigned = std::move(moved);
    REQUIRE(moveAssigned.size() == 3);
    REQUIRE(moved.empty());

    std::string joined;
    source.forEach([&joined](const std::string& item) { joined += item; });
    REQUIRE(joined == "ab");
}
