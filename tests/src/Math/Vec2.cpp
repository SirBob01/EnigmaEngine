#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>
#include <unordered_set>

TEST_CASE("Vec2 length squared", "[Vec2]") {
    Dynamo::Vec2 a(3, 4);
    REQUIRE(a.length_squared() == 25);
}

TEST_CASE("Vec2 length", "[Vec2]") {
    Dynamo::Vec2 a(3, 4);
    REQUIRE(a.length() == 5);
}

TEST_CASE("Vec2 left normal", "[Vec2]") {
    Dynamo::Vec2 a(3, 4);
    Dynamo::Vec2 b = a.left_normal();
    REQUIRE(b.x == -a.y);
    REQUIRE(b.y == a.x);
}

TEST_CASE("Vec2 right normal", "[Vec2]") {
    Dynamo::Vec2 a(3, 4);
    Dynamo::Vec2 b = a.right_normal();
    REQUIRE(b.x == a.y);
    REQUIRE(b.y == -a.x);
}

TEST_CASE("Vec2 add", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    Dynamo::Vec2 b(3, 4);
    Dynamo::Vec2 c = a + b;
    REQUIRE(c.x == 4);
    REQUIRE(c.y == 6);
}

TEST_CASE("Vec2 subtract", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    Dynamo::Vec2 b(3, 4);
    Dynamo::Vec2 c = a - b;
    REQUIRE(c.x == -2);
    REQUIRE(c.y == -2);
}

TEST_CASE("Vec2 multiply by scalar", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    Dynamo::Vec2 b = a * 3.2;
    REQUIRE(b.x == 3.2f);
    REQUIRE(b.y == 6.4f);
}

TEST_CASE("Vec2 divide by scalar", "[Vec2]") {
    Dynamo::Vec2 a(3.2f, 6.4f);
    Dynamo::Vec2 b = a / 3.2f;
    REQUIRE(b.x == 1);
    REQUIRE(b.y == 2);
}

TEST_CASE("Vec2 negation", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    Dynamo::Vec2 b = -a;
    REQUIRE(b.x == -1);
    REQUIRE(b.y == -2);
}

TEST_CASE("Vec2 add in-place", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    Dynamo::Vec2 b(3, 4);
    a += b;
    REQUIRE(a.x == 4);
    REQUIRE(a.y == 6);
}

TEST_CASE("Vec2 subtract in-place", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    Dynamo::Vec2 b(3, 4);
    a -= b;
    REQUIRE(a.x == -2);
    REQUIRE(a.y == -2);
}

TEST_CASE("Vec2 multiply by scalar in-place", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    a *= 3.2;
    REQUIRE(a.x == 3.2f);
    REQUIRE(a.y == 6.4f);
}

TEST_CASE("Vec2 divide by scalar in-place", "[Vec2]") {
    Dynamo::Vec2 a(3.2f, 6.4f);
    a /= 3.2f;
    REQUIRE(a.x == 1);
    REQUIRE(a.y == 2);
}

TEST_CASE("Vec2 normalize", "[Vec2]") {
    Dynamo::Vec2 a(3, 4);
    a.normalize();
    REQUIRE(a.x == 3.0f / 5);
    REQUIRE(a.y == 4.0f / 5);
}

TEST_CASE("Vec2 dot product", "[Vec2]") {
    Dynamo::Vec2 a(3, 4);
    Dynamo::Vec2 b(1, -2);
    float dot = a * b;
    REQUIRE(dot == -5);
}

TEST_CASE("Vec2 cross product", "[Vec2]") {
    Dynamo::Vec2 a(3, 4);
    Dynamo::Vec2 b(1, -2);
    float cross = a.cross(b);
    REQUIRE(cross == -10);
}

TEST_CASE("Vec2 equality", "[Vec2]") {
    Dynamo::Vec2 a(-3.2, 6.4);
    Dynamo::Vec2 b(-3.2, 6.4);
    REQUIRE(a == b);
}

TEST_CASE("Vec2 inequality", "[Vec2]") {
    Dynamo::Vec2 a(1, 2);
    Dynamo::Vec2 b(3, 2);
    Dynamo::Vec2 c(1, 4);
    Dynamo::Vec2 d = b + c;
    REQUIRE(a != b);
    REQUIRE(a != c);
    REQUIRE(a != d);
}

TEST_CASE("Vec2 hash", "[Vec2]") { REQUIRE_NOTHROW(std::unordered_set<Dynamo::Vec2>()); }
