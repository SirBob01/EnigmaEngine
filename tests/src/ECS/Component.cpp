#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ECS::ComponentRegistry get", "[ECS::ComponentRegistry]") {
    REQUIRE(Dynamo::ECS::ComponentRegistry::get<int>() == 0);
    REQUIRE(Dynamo::ECS::ComponentRegistry::get<char>() == 1);
    REQUIRE(Dynamo::ECS::ComponentRegistry::get<int>() == 0);
    REQUIRE(Dynamo::ECS::ComponentRegistry::get<Dynamo::Vec2>() == 2);
    REQUIRE(Dynamo::ECS::ComponentRegistry::get<float>() == 3);
    REQUIRE(Dynamo::ECS::ComponentRegistry::get<float>() == 3);
}
