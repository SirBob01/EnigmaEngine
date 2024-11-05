#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

uintptr_t ids = 0;

TEST_CASE("ECS::SparsePool get", "[ECS::SparsePool]") {
    Dynamo::ECS::SparsePool set;
    set.initialize(sizeof(char));

    Dynamo::ECS::Entity a = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    Dynamo::ECS::Entity b = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    Dynamo::ECS::Entity c = reinterpret_cast<Dynamo::ECS::Entity>(ids++);

    set.insert<char>(a, 'a');
    set.insert<char>(b, 'b');
    set.insert<char>(c, 'c');

    REQUIRE(set.get<char>(a) == 'a');
    REQUIRE(set.get<char>(b) == 'b');
    REQUIRE(set.get<char>(c) == 'c');
}

TEST_CASE("ECS::SparsePool insert", "[ECS::SparsePool]") {
    Dynamo::ECS::SparsePool set;
    set.initialize(sizeof(char));

    Dynamo::ECS::Entity a = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(a, 'a');
    REQUIRE(set.get<char>(a) == 'a');

    Dynamo::ECS::Entity b = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(b, 'b');
    REQUIRE(set.get<char>(b) == 'b');

    Dynamo::ECS::Entity c = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(c, 'c');
    REQUIRE(set.get<char>(c) == 'c');

    REQUIRE(set.size() == 3);
}

TEST_CASE("ECS::SparsePool remove", "[ECS::SparsePool]") {
    Dynamo::ECS::SparsePool set;
    set.initialize(sizeof(char));

    Dynamo::ECS::Entity a = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(a, 'a');
    REQUIRE(set.get<char>(a) == 'a');

    set.remove(a);
    REQUIRE(!set.exists(a));
    REQUIRE_THROWS(set.get<char>(a));

    REQUIRE(set.size() == 0);
}

TEST_CASE("ECS::SparsePool clear", "[ECS::SparsePool]") {
    Dynamo::ECS::SparsePool set;
    set.initialize(sizeof(char));

    Dynamo::ECS::Entity a = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(a, 'a');
    REQUIRE(set.get<char>(a) == 'a');

    Dynamo::ECS::Entity b = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(b, 'b');
    REQUIRE(set.get<char>(b) == 'b');

    Dynamo::ECS::Entity c = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(c, 'c');
    REQUIRE(set.get<char>(c) == 'c');

    set.clear();

    REQUIRE(set.size() == 0);

    REQUIRE(!set.exists(a));
    REQUIRE(!set.exists(b));
    REQUIRE(!set.exists(c));

    REQUIRE_THROWS(set.get<char>(a));
    REQUIRE_THROWS(set.get<char>(b));
    REQUIRE_THROWS(set.get<char>(c));
}

TEST_CASE("ECS::SparsePool foreach", "[ECS::SparsePool]") {
    Dynamo::ECS::SparsePool set;
    set.initialize(sizeof(char));

    Dynamo::ECS::Entity a = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(a, 'a');
    REQUIRE(set.get<char>(a) == 'a');

    Dynamo::ECS::Entity b = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(b, 'b');
    REQUIRE(set.get<char>(b) == 'b');

    Dynamo::ECS::Entity c = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(c, 'c');
    REQUIRE(set.get<char>(c) == 'c');

    set.remove(a);
    Dynamo::ECS::Entity d = reinterpret_cast<Dynamo::ECS::Entity>(ids++);
    set.insert<char>(d, 'd');

    REQUIRE(set.size() == 3);

    struct Pair {
        char item;
        Dynamo::ECS::Entity id;
    };
    std::vector<Pair> pairs;
    set.foreach<char>([&](Dynamo::ECS::Entity id, char item) { pairs.push_back({item, id}); });

    REQUIRE(pairs[0].item == 'c');
    REQUIRE(pairs[0].id == c);

    REQUIRE(pairs[1].item == 'b');
    REQUIRE(pairs[1].id == b);

    REQUIRE(pairs[2].item == 'd');
    REQUIRE(pairs[2].id == d);

    REQUIRE(pairs.size() == set.size());
}