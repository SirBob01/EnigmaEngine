#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Mat4 basis", "[Mat4]") {
    Dynamo::Vec3 x(3, 2, 1);
    Dynamo::Vec3 y(4, 8, 6);
    Dynamo::Vec3 z(7, 5, 9);
    Dynamo::Mat4 m(x, y, z);

    REQUIRE(m.values[0] == 3);
    REQUIRE(m.values[1] == 2);
    REQUIRE(m.values[2] == 1);
    REQUIRE(m.values[3] == 0);
    REQUIRE(m.values[4] == 4);
    REQUIRE(m.values[5] == 8);
    REQUIRE(m.values[6] == 6);
    REQUIRE(m.values[7] == 0);
    REQUIRE(m.values[8] == 7);
    REQUIRE(m.values[9] == 5);
    REQUIRE(m.values[10] == 9);
    REQUIRE(m.values[11] == 0);
    REQUIRE(m.values[12] == 0);
    REQUIRE(m.values[13] == 0);
    REQUIRE(m.values[14] == 0);
    REQUIRE(m.values[15] == 1);
}

TEST_CASE("Mat4 compose", "[Mat4]") {
    Dynamo::Vec3 position(3, 2, 1);
    Dynamo::Quaternion rotation(2, 1, 6, 1);
    Dynamo::Vec3 scale(2, 1, 6);
    Dynamo::Mat4 m(position, rotation, scale);

    REQUIRE(m.values[0] == -146);
    REQUIRE(m.values[1] == 32);
    REQUIRE(m.values[2] == 44);
    REQUIRE(m.values[3] == 0);
    REQUIRE(m.values[4] == -8);
    REQUIRE(m.values[5] == -79);
    REQUIRE(m.values[6] == 16);
    REQUIRE(m.values[7] == 0);
    REQUIRE(m.values[8] == 156);
    REQUIRE(m.values[9] == 48);
    REQUIRE(m.values[10] == -54);
    REQUIRE(m.values[11] == 0);
    REQUIRE(m.values[12] == 3);
    REQUIRE(m.values[13] == 2);
    REQUIRE(m.values[14] == 1);
    REQUIRE(m.values[15] == 1);
}

TEST_CASE("Mat4 add", "[Mat4]") {
    Dynamo::Mat4 a;
    Dynamo::Mat4 b;
    Dynamo::Mat4 c = a + b;
    REQUIRE(c.values[0] == 2);
    REQUIRE(c.values[1] == 0);
    REQUIRE(c.values[2] == 0);
    REQUIRE(c.values[3] == 0);

    REQUIRE(c.values[4] == 0);
    REQUIRE(c.values[5] == 2);
    REQUIRE(c.values[6] == 0);
    REQUIRE(c.values[7] == 0);

    REQUIRE(c.values[8] == 0);
    REQUIRE(c.values[9] == 0);
    REQUIRE(c.values[10] == 2);
    REQUIRE(c.values[11] == 0);

    REQUIRE(c.values[12] == 0);
    REQUIRE(c.values[13] == 0);
    REQUIRE(c.values[14] == 0);
    REQUIRE(c.values[15] == 2);
}

TEST_CASE("Mat4 subtract", "[Mat4]") {
    Dynamo::Mat4 a;
    Dynamo::Mat4 b;
    Dynamo::Mat4 c = a - b;
    Dynamo::Mat4 d(0);
    REQUIRE(c == d);
}

TEST_CASE("Mat4 multiply", "[Mat4]") {
    Dynamo::Vec3 x(3, 2, 1);
    Dynamo::Vec3 y(4, 8, 6);
    Dynamo::Vec3 z(7, 5, 9);

    Dynamo::Mat4 a(x, y, z);
    Dynamo::Mat4 b(z, x, y);

    // Identity
    Dynamo::Mat4 i;
    REQUIRE(a * i == a);
    REQUIRE(b * i == b);

    Dynamo::Mat4 c = a * b;
    REQUIRE(c.values[0] == 104);
    REQUIRE(c.values[1] == 99);
    REQUIRE(c.values[2] == 118);
    REQUIRE(c.values[3] == 0);
    REQUIRE(c.values[4] == 24);
    REQUIRE(c.values[5] == 27);
    REQUIRE(c.values[6] == 24);
    REQUIRE(c.values[7] == 0);
    REQUIRE(c.values[8] == 86);
    REQUIRE(c.values[9] == 102);
    REQUIRE(c.values[10] == 106);
    REQUIRE(c.values[11] == 0);
    REQUIRE(c.values[12] == 0);
    REQUIRE(c.values[13] == 0);
    REQUIRE(c.values[14] == 0);
    REQUIRE(c.values[15] == 1);
}

TEST_CASE("Mat4 multiply by scalar", "[Mat4]") {
    Dynamo::Mat4 a(2);
    Dynamo::Mat4 b = a * 3.2;
    Dynamo::Mat4 c(6.4);
    REQUIRE(b == c);
}

TEST_CASE("Mat4 multiply Vec3", "[Vec3]") {
    Dynamo::Vec3 position(3, 2, 1);
    Dynamo::Quaternion rotation(2, 1, 6, 1);
    Dynamo::Vec3 scale(2, 1, 6);
    Dynamo::Mat4 m(position, rotation, scale);
    Dynamo::Vec3 a(1, 2, 4);
    a = m * a;
    REQUIRE(a.x == 465);
    REQUIRE(a.y == 68);
    REQUIRE(a.z == -139);
}

TEST_CASE("Mat4 divide by scalar", "[Mat4]") {
    Dynamo::Mat4 a(6.4f);
    Dynamo::Mat4 b = a / 3.2f;
    Dynamo::Mat4 c(2);
    REQUIRE(b == c);
}

TEST_CASE("Mat4 negation", "[Mat4]") {
    Dynamo::Mat4 a(2);
    Dynamo::Mat4 b = -a;
    Dynamo::Mat4 c(-2);
    REQUIRE(b == c);
}

TEST_CASE("Mat4 add in-place", "[Mat4]") {
    Dynamo::Mat4 a;
    Dynamo::Mat4 b;
    a += b;
    REQUIRE(a.values[0] == 2);
    REQUIRE(a.values[1] == 0);
    REQUIRE(a.values[2] == 0);
    REQUIRE(a.values[3] == 0);

    REQUIRE(a.values[4] == 0);
    REQUIRE(a.values[5] == 2);
    REQUIRE(a.values[6] == 0);
    REQUIRE(a.values[7] == 0);

    REQUIRE(a.values[8] == 0);
    REQUIRE(a.values[9] == 0);
    REQUIRE(a.values[10] == 2);
    REQUIRE(a.values[11] == 0);

    REQUIRE(a.values[12] == 0);
    REQUIRE(a.values[13] == 0);
    REQUIRE(a.values[14] == 0);
    REQUIRE(a.values[15] == 2);
}

TEST_CASE("Mat4 subtract in-place", "[Mat4]") {
    Dynamo::Mat4 a;
    Dynamo::Mat4 b;
    Dynamo::Mat4 d(0);
    a -= b;
    REQUIRE(a == d);
}

TEST_CASE("Mat4 multiply in-place", "[Mat4]") {
    Dynamo::Vec3 x(3, 2, 1);
    Dynamo::Vec3 y(4, 8, 6);
    Dynamo::Vec3 z(7, 5, 9);

    Dynamo::Mat4 a(x, y, z);
    Dynamo::Mat4 b(z, x, y);

    Dynamo::Mat4 c = a * b;
    a *= b;

    REQUIRE(a == c);
}

TEST_CASE("Mat4 multiply by scalar in-place", "[Mat4]") {
    Dynamo::Mat4 a(2);
    Dynamo::Mat4 b(6.4);
    a *= 3.2;
    REQUIRE(a == b);
}

TEST_CASE("Mat4 divide by scalar in-place", "[Mat4]") {
    Dynamo::Mat4 a(6.4f);
    Dynamo::Mat4 b(2);
    a /= 3.2f;
    REQUIRE(a == b);
}

TEST_CASE("Mat4 transpose", "[Mat4]") {
    Dynamo::Vec3 x(3, 2, 1);
    Dynamo::Vec3 y(4, 8, 6);
    Dynamo::Vec3 z(7, 5, 9);
    Dynamo::Mat4 m(x, y, z);
    m.transpose();

    REQUIRE(m.values[0] == 3);
    REQUIRE(m.values[1] == 4);
    REQUIRE(m.values[2] == 7);
    REQUIRE(m.values[3] == 0);
    REQUIRE(m.values[4] == 2);
    REQUIRE(m.values[5] == 8);
    REQUIRE(m.values[6] == 5);
    REQUIRE(m.values[7] == 0);
    REQUIRE(m.values[8] == 1);
    REQUIRE(m.values[9] == 6);
    REQUIRE(m.values[10] == 9);
    REQUIRE(m.values[11] == 0);
    REQUIRE(m.values[12] == 0);
    REQUIRE(m.values[13] == 0);
    REQUIRE(m.values[14] == 0);
    REQUIRE(m.values[15] == 1);
}

TEST_CASE("Mat4 equality", "[Mat4]") {
    Dynamo::Mat4 a(-3.2);
    Dynamo::Mat4 b(-3.2);
    REQUIRE(a == b);
}

TEST_CASE("Mat4 inequality", "[Mat4]") {
    Dynamo::Mat4 a;
    Dynamo::Mat4 b(2);
    Dynamo::Mat4 c(4);
    Dynamo::Mat4 d = b + c;
    REQUIRE(a != b);
    REQUIRE(a != c);
    REQUIRE(a != d);
}
