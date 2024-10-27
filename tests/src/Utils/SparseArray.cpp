#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

DYN_DEFINE_ID_TYPE(Id);

using CharArray = Dynamo::SparseArray<Id, char>;

TEST_CASE("SparseArray get", "[SparseArray]") {
    CharArray set;

    Id a = set.insert('a');
    Id b = set.insert('b');
    Id c = set.insert('c');

    REQUIRE(set.get(a) == 'a');
    REQUIRE(set.get(b) == 'b');
    REQUIRE(set.get(c) == 'c');
}

TEST_CASE("SparseArray const get", "[SparseArray]") {
    CharArray set;

    Id a = set.insert('a');
    Id b = set.insert('b');
    Id c = set.insert('c');

    const CharArray &const_set = set;

    REQUIRE(const_set.get(a) == 'a');
    REQUIRE(const_set.get(b) == 'b');
    REQUIRE(const_set.get(c) == 'c');
}

TEST_CASE("SparseArray insert", "[SparseArray]") {
    CharArray set;

    Id a = set.insert('a');
    Id b = set.insert('b');
    Id c = set.insert('c');

    const CharArray &const_set = set;

    REQUIRE(const_set.get(a) == 'a');
    REQUIRE(const_set.get(b) == 'b');
    REQUIRE(const_set.get(c) == 'c');

    REQUIRE(set.size() == 3);
    REQUIRE(!set.empty());
}

TEST_CASE("SparseArray remove", "[SparseArray]") {
    CharArray set;

    Id a = set.insert('a');
    Id b = set.insert('b');
    Id c = set.insert('c');

    REQUIRE(set.get(a) == 'a');
    REQUIRE(set.exists(a));

    REQUIRE(set.size() == 3);
    REQUIRE(!set.empty());

    set.remove(a);

    REQUIRE_THROWS(set.get(a));
    REQUIRE(!set.exists(a));

    REQUIRE(set.size() == 2);
    REQUIRE(!set.empty());

    Id d = set.insert('d');

    REQUIRE(set.get(b) == 'b');
    REQUIRE(set.get(c) == 'c');
    REQUIRE(set.get(d) == 'd');

    set.remove(b);
    set.remove(c);
    set.remove(d);

    REQUIRE_THROWS(set.get(b));
    REQUIRE_THROWS(set.get(c));
    REQUIRE_THROWS(set.get(d));

    REQUIRE(set.size() == 0);
    REQUIRE(set.empty());
}

TEST_CASE("SparseArray clear", "[SparseArray]") {
    CharArray set;

    Id a = set.insert('a');
    Id b = set.insert('b');
    Id c = set.insert('c');

    set.clear();

    REQUIRE(set.size() == 0);
    REQUIRE(set.empty());

    REQUIRE(!set.exists(a));
    REQUIRE(!set.exists(b));
    REQUIRE(!set.exists(c));

    REQUIRE_THROWS(set.get(a));
    REQUIRE_THROWS(set.get(b));
    REQUIRE_THROWS(set.get(c));
}

TEST_CASE("SparseArray foreach", "[SparseArray]") {
    CharArray set;

    set.insert('a');
    set.insert('b');
    set.insert('c');

    std::vector<char> items;

    set.foreach ([&](char &item) { items.push_back(item); });

    REQUIRE(items[0] == 'a');
    REQUIRE(items[1] == 'b');
    REQUIRE(items[2] == 'c');

    REQUIRE(items.size() == set.size());
}