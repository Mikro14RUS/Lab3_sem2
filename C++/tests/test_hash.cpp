#include "catch.hpp"

#include <string>
#include <vector>

#include "HashTable.hpp"

using lab3::HashTable;

TEST_CASE("HashTable: добавление, поиск и обновление") {
    HashTable table(4);
    REQUIRE(table.empty());
    REQUIRE(table.size() == 0);
    REQUIRE(table.toString() == "{}");

    REQUIRE(table.put("b", "2"));
    REQUIRE(table.put("a", "1"));
    REQUIRE(table.size() == 2);
    REQUIRE_FALSE(table.put("a", "10"));
    REQUIRE(table.size() == 2);

    std::string value;
    REQUIRE(table.get("a", value));
    REQUIRE(value == "10");
    REQUIRE(table.get("b", value));
    REQUIRE(value == "2");
    REQUIRE_FALSE(table.get("missing", value));
    REQUIRE(table.contains("a"));
    REQUIRE_FALSE(table.contains("missing"));

    REQUIRE(table.toString() == "{a=10, b=2}");
    const auto entries = table.entries();
    REQUIRE(entries.size() == 2);
    REQUIRE(entries[0].first == "a");
    REQUIRE(entries[1].first == "b");
}

TEST_CASE("HashTable: удаление и очистка") {
    HashTable table;
    std::string removed;
    REQUIRE_FALSE(table.remove("missing", removed));

    table.put("a", "1");
    table.put("b", "2");
    REQUIRE(table.remove("a", removed));
    REQUIRE(removed == "1");
    REQUIRE_FALSE(table.contains("a"));
    REQUIRE(table.size() == 1);
    table.clear();
    REQUIRE(table.empty());
    REQUIRE(table.collisions() == 0);
}

TEST_CASE("HashTable: перестроение и статистика") {
    HashTable table(2);
    for (int i = 0; i < 500; ++i) {
        REQUIRE(table.put("key_" + std::to_string(i), std::to_string(i)));
    }
    REQUIRE(table.size() == 500);
    REQUIRE(table.bucketCount() > 2);
    REQUIRE(table.loadFactor() > 0.0);
    REQUIRE(table.loadFactor() <= 0.75);
    REQUIRE(table.maxChainLength() >= 1);
    REQUIRE(table.collisions() > 0);

    std::string value;
    for (int i = 0; i < 500; ++i) {
        REQUIRE(table.get("key_" + std::to_string(i), value));
        REQUIRE(value == std::to_string(i));
    }

    std::size_t count = 0;
    table.forEach([&count](const std::string&, const std::string&) { ++count; });
    REQUIRE(count == 500);
    REQUIRE(HashTable::hashOf("abc") == HashTable::hashOf("abc"));
}
