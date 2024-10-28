#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VirtualMemory reserve", "[VirtualMemory]") {
    Dynamo::VirtualMemory memory(256, 4);
    memory.reserve(3);
    unsigned offset = memory.reserve(12);

    REQUIRE(offset == 4);
    REQUIRE(memory.size(offset) == 12);
    REQUIRE(memory.get_mapped(offset) != nullptr);
}

TEST_CASE("VirtualMemory free", "[VirtualMemory]") {
    Dynamo::VirtualMemory memory(256, 4);
    unsigned offset = memory.reserve(3);

    REQUIRE(offset == 0);
    REQUIRE(memory.size(offset) == 3);
    REQUIRE(memory.get_mapped(offset) != nullptr);

    memory.free(offset);
    REQUIRE_THROWS(memory.size(offset));
}
