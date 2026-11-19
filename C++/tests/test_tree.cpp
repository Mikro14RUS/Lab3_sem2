#include "catch.hpp"

#include <optional>
#include <string>
#include <vector>

#include "FullBinaryTree.hpp"

using lab3::FullBinaryTree;

TEST_CASE("FullBinaryTree: вставка, поиск и размер") {
    FullBinaryTree tree;
    REQUIRE(tree.empty());
    REQUIRE(tree.size() == 0);
    REQUIRE(tree.isFull());
    REQUIRE(tree.isBst());
    REQUIRE(tree.toString() == "Дерево пусто");

    REQUIRE(tree.insert(7));
    REQUIRE(tree.insert(3));
    REQUIRE(tree.insert(9));
    REQUIRE_FALSE(tree.insert(7));
    REQUIRE(tree.size() == 3);
    REQUIRE(tree.find(3));
    REQUIRE(tree.find(9));
    REQUIRE_FALSE(tree.find(5));
    REQUIRE(tree.isFull());
    REQUIRE(tree.isBst());
    REQUIRE(tree.toString() == "BFS: 7 3 9");
    REQUIRE(tree.toString(FullBinaryTree::Traversal::Preorder) == "Preorder: 7 3 9");
    REQUIRE(tree.toString(FullBinaryTree::Traversal::Inorder) == "Inorder: 3 7 9");
    REQUIRE(tree.toString(FullBinaryTree::Traversal::Postorder) == "Postorder: 3 9 7");
    REQUIRE(tree.toString(FullBinaryTree::Traversal::Pretty).find("└── 7") !=
            std::string::npos);
}

TEST_CASE("FullBinaryTree: свойство Full и обходы") {
    FullBinaryTree tree;
    tree.insert(7);
    tree.insert(3);
    tree.insert(2);
    tree.insert(9);
    tree.insert(8);
    REQUIRE_FALSE(tree.isFull());
    REQUIRE(tree.toString() == "BFS: 7 3 9 2 8");
    REQUIRE(tree.preorder() == std::vector<int>{7, 3, 2, 9, 8});
    REQUIRE(tree.inorder() == std::vector<int>{2, 3, 7, 8, 9});
    REQUIRE(tree.postorder() == std::vector<int>{2, 3, 8, 9, 7});
}

TEST_CASE("FullBinaryTree: сериализация preorder с маркерами") {
    FullBinaryTree tree;
    tree.insert(7);
    tree.insert(3);
    tree.insert(9);
    const auto tokens = tree.preorderWithNulls();
    REQUIRE(tokens == std::vector<std::optional<int>>{7, 3, std::nullopt, std::nullopt, 9, std::nullopt, std::nullopt});

    FullBinaryTree restored;
    REQUIRE(restored.loadPreorderWithNulls(tokens));
    REQUIRE(restored.size() == 3);
    REQUIRE(restored.toString() == "BFS: 7 3 9");
    REQUIRE(restored.isFull());

    FullBinaryTree empty;
    REQUIRE(empty.loadPreorderWithNulls({}));
    REQUIRE(empty.empty());
    REQUIRE(empty.loadPreorderWithNulls({std::nullopt}));
    REQUIRE(empty.empty());
}

TEST_CASE("FullBinaryTree: некорректные данные и сохранение состояния") {
    FullBinaryTree tree;
    tree.insert(7);
    tree.insert(3);
    tree.insert(9);

    REQUIRE_FALSE(tree.loadPreorderWithNulls({7, 3}));
    REQUIRE_FALSE(tree.loadPreorderWithNulls({7, 3, 9}));
    REQUIRE_FALSE(tree.loadPreorderWithNulls({7, 3, std::nullopt, 9, std::nullopt,
                                              std::nullopt, std::nullopt, std::nullopt}));
    REQUIRE_FALSE(tree.loadPreorderWithNulls({8, 9, std::nullopt, std::nullopt,
                                              std::nullopt}));
    REQUIRE(tree.toString() == "BFS: 7 3 9");
}

TEST_CASE("FullBinaryTree: копирование, перемещение и очистка") {
    FullBinaryTree source;
    source.insert(7);
    source.insert(3);
    source.insert(9);

    FullBinaryTree copy(source);
    REQUIRE(copy.toString() == "BFS: 7 3 9");
    copy.insert(1);
    REQUIRE(source.size() == 3);

    FullBinaryTree assigned;
    assigned = source;
    REQUIRE(assigned.size() == 3);
    assigned = assigned;

    FullBinaryTree moved(std::move(copy));
    REQUIRE(moved.size() == 4);
    REQUIRE(copy.empty());
    FullBinaryTree moveAssigned;
    moveAssigned = std::move(moved);
    REQUIRE(moveAssigned.size() == 4);
    REQUIRE(moved.empty());

    source.clear();
    REQUIRE(source.empty());
    REQUIRE(source.isFull());
}
