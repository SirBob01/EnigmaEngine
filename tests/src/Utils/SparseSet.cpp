#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

DYN_DEFINE_ID_TYPE(Id);
uintptr_t ids = 0;

using CharSet = Dynamo::SparseSet<Id, char>;

TEST_CASE("SparseSet []", "[SparseSet]") {
    CharSet set;

    set.insert(reinterpret_cast<Id>(ids++), 'a');
    set.insert(reinterpret_cast<Id>(ids++), 'b');
    set.insert(reinterpret_cast<Id>(ids++), 'c');

    REQUIRE(set[0] == 'a');
    REQUIRE(set[1] == 'b');
    REQUIRE(set[2] == 'c');
}

TEST_CASE("SparseSet const []", "[SparseSet]") {
    CharSet set;

    set.insert(reinterpret_cast<Id>(ids++), 'a');
    set.insert(reinterpret_cast<Id>(ids++), 'b');
    set.insert(reinterpret_cast<Id>(ids++), 'c');

    const CharSet &const_set = set;

    REQUIRE(const_set[0] == 'a');
    REQUIRE(const_set[1] == 'b');
    REQUIRE(const_set[2] == 'c');
}

TEST_CASE("SparseSet get", "[SparseSet]") {
    CharSet set;

    Id a = reinterpret_cast<Id>(ids++);
    Id b = reinterpret_cast<Id>(ids++);
    Id c = reinterpret_cast<Id>(ids++);

    set.insert(a, 'a');
    set.insert(b, 'b');
    set.insert(c, 'c');

    REQUIRE(set.get(a) == 'a');
    REQUIRE(set.get(b) == 'b');
    REQUIRE(set.get(c) == 'c');
}

TEST_CASE("SparseSet const get", "[SparseSet]") {
    CharSet set;

    Id a = reinterpret_cast<Id>(ids++);
    Id b = reinterpret_cast<Id>(ids++);
    Id c = reinterpret_cast<Id>(ids++);

    set.insert(a, 'a');
    set.insert(b, 'b');
    set.insert(c, 'c');

    const CharSet &const_set = set;

    REQUIRE(const_set.get(a) == 'a');
    REQUIRE(const_set.get(b) == 'b');
    REQUIRE(const_set.get(c) == 'c');
}

TEST_CASE("SparseSet insert", "[SparseSet]") {
    CharSet set;

    Id a = reinterpret_cast<Id>(ids++);
    set.insert(a, 'a');
    REQUIRE(set.get(a) == 'a');

    Id b = reinterpret_cast<Id>(ids++);
    set.insert(b, 'b');
    REQUIRE(set.get(b) == 'b');

    Id c = reinterpret_cast<Id>(ids++);
    set.insert(c, 'c');
    REQUIRE(set.get(c) == 'c');

    REQUIRE(set.size() == 3);
    REQUIRE(!set.empty());
}

TEST_CASE("SparseSet remove", "[SparseSet]") {
    CharSet set;

    Id a = reinterpret_cast<Id>(ids++);
    set.insert(a, 'a');
    REQUIRE(set.find(a) == 0);
    REQUIRE(set.get(a) == 'a');

    set.remove(a);
    REQUIRE(set.find(a) == -1);
    REQUIRE(!set.exists(a));
    REQUIRE_THROWS(set.get(a));

    REQUIRE(set.size() == 0);
    REQUIRE(set.empty());
}

TEST_CASE("SparseSet clear", "[SparseSet]") {
    CharSet set;

    Id a = reinterpret_cast<Id>(ids++);
    set.insert(a, 'a');
    REQUIRE(set.get(a) == 'a');

    Id b = reinterpret_cast<Id>(ids++);
    set.insert(b, 'b');
    REQUIRE(set.get(b) == 'b');

    Id c = reinterpret_cast<Id>(ids++);
    set.insert(c, 'c');
    REQUIRE(set.get(c) == 'c');

    set.clear();

    REQUIRE(set.size() == 0);
    REQUIRE(set.empty());

    REQUIRE(!set.exists(a));
    REQUIRE(!set.exists(b));
    REQUIRE(!set.exists(c));

    REQUIRE(set.find(a) == -1);
    REQUIRE(set.find(b) == -1);
    REQUIRE(set.find(c) == -1);

    REQUIRE_THROWS(set.get(a));
    REQUIRE_THROWS(set.get(b));
    REQUIRE_THROWS(set.get(c));
}

TEST_CASE("SparseSet foreach", "[SparseSet]") {
    CharSet set;

    Id a = reinterpret_cast<Id>(ids++);
    set.insert(a, 'a');
    REQUIRE(set.get(a) == 'a');

    Id b = reinterpret_cast<Id>(ids++);
    set.insert(b, 'b');
    REQUIRE(set.get(b) == 'b');

    Id c = reinterpret_cast<Id>(ids++);
    set.insert(c, 'c');
    REQUIRE(set.get(c) == 'c');

    set.remove(a);
    Id d = reinterpret_cast<Id>(ids++);
    set.insert(d, 'd');

    REQUIRE(set.size() == 3);
    REQUIRE(!set.empty());

    struct Pair {
        char &item;
        Id id;
    };
    std::vector<Pair> triplets;
    std::vector<char> items;
    std::vector<Id> ids;

    set.foreach ([&](char &item, Id id) { triplets.push_back({item, id}); });
    set.foreach_items([&](char &item) { items.push_back(item); });
    set.foreach_ids([&](Id id) { ids.push_back(id); });

    REQUIRE(triplets[0].item == 'c');
    REQUIRE(triplets[0].id == c);
    REQUIRE(items[0] == 'c');
    REQUIRE(ids[0] == c);

    REQUIRE(triplets[1].item == 'b');
    REQUIRE(triplets[1].id == b);
    REQUIRE(items[1] == 'b');
    REQUIRE(ids[1] == b);

    REQUIRE(triplets[2].item == 'd');
    REQUIRE(triplets[2].id == d);
    REQUIRE(items[2] == 'd');
    REQUIRE(ids[2] == d);

    REQUIRE(triplets.size() == set.size());
    REQUIRE(items.size() == set.size());
    REQUIRE(ids.size() == set.size());
}