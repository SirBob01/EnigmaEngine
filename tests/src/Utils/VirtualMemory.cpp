#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VirtualMemory reserve", "[VirtualMemory]") {
    Dynamo::VirtualMemory memory(256, 4);
    memory.reserve(3);
    unsigned offset = memory.reserve(12).value();

    REQUIRE(offset == 4);
    REQUIRE(memory.size(offset) == 12);
    REQUIRE(memory.mapped(offset) != nullptr);
    REQUIRE_THROWS(memory.reserve(1024).value());
}

TEST_CASE("VirtualMemory free", "[VirtualMemory]") {
    Dynamo::VirtualMemory memory(256, 4);
    unsigned offset = memory.reserve(3).value();

    REQUIRE(offset == 0);
    REQUIRE(memory.size(offset) == 3);
    REQUIRE(memory.mapped(offset) != nullptr);

    memory.free(offset);
    REQUIRE_THROWS(memory.size(offset));
}

TEST_CASE("VirtualMemory grow", "[VirtualMemory]") {
    Dynamo::VirtualMemory memory(256, 4);
    memory.reserve(3).value();
    memory.grow(2048);

    REQUIRE(memory.capacity() == 2048);
    REQUIRE_NOTHROW(memory.reserve(1024).value());
}
