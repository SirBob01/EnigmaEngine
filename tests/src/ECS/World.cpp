#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ECS::World entity creation and destruction", "[ECS::World]") {
    Dynamo::ECS::World world;
    Dynamo::ECS::Entity entity0 = world.create();
    world.add<Dynamo::Vec2>(entity0, 1, 2);

    Dynamo::ECS::Entity entity1 = world.create();
    world.add<Dynamo::Vec2>(entity1, 3, 4);

    world.destroy(entity0);
    REQUIRE(!world.get_safe<Dynamo::Vec2>(entity0).has_value());
    REQUIRE_THROWS(world.get<Dynamo::Vec2>(entity0));

    Dynamo::ECS::Entity entity2 = world.create();
    world.add<Dynamo::Vec2>(entity2, 5, 6);

    Dynamo::ECS::Entity entity3 = world.create();
    world.add<Dynamo::Vec2>(entity3, 7, 8);

    DYN_ASSERT(entity0 == reinterpret_cast<Dynamo::ECS::Entity>(0));
    DYN_ASSERT(entity1 == reinterpret_cast<Dynamo::ECS::Entity>(1));
    DYN_ASSERT(entity2 == reinterpret_cast<Dynamo::ECS::Entity>(0));
    DYN_ASSERT(entity3 == reinterpret_cast<Dynamo::ECS::Entity>(2));
}

TEST_CASE("ECS::World component add and remove", "[ECS::World]") {
    Dynamo::ECS::World world;
    Dynamo::ECS::Entity entity = world.create();
    world.add<Dynamo::Vec2>(entity, 12, 4);
    world.add<Dynamo::Vec3>(entity, 69, 3, 2);

    // Failure case
    REQUIRE(!world.get_safe<float>(entity).has_value());
    REQUIRE_THROWS(world.get<float>(entity));

    // Initialized from object
    REQUIRE(world.get<Dynamo::Vec2>(entity).x == 12);
    REQUIRE(world.get<Dynamo::Vec2>(entity).y == 4);

    // Initialized from args
    REQUIRE(world.get<Dynamo::Vec3>(entity).x == 69);
    REQUIRE(world.get<Dynamo::Vec3>(entity).y == 3);
    REQUIRE(world.get<Dynamo::Vec3>(entity).z == 2);

    world.remove<Dynamo::Vec2>(entity);

    // Removal
    REQUIRE(!world.get_safe<Dynamo::Vec2>(entity).has_value());
    REQUIRE_THROWS(world.get<Dynamo::Vec2>(entity));
}

TEST_CASE("ECS::World foreach", "[ECS::World]") {
    Dynamo::ECS::World world;

    struct Object {
        Dynamo::ECS::Entity id;
        Dynamo::Vec2 v2;
    };
    std::vector<Object> ground_truth;

    for (unsigned i = 0; i < 100; i++) {
        Object object;
        object.id = world.create();
        object.v2 = {
            Dynamo::Random::random(),
            Dynamo::Random::random(),
        };

        world.add<Dynamo::Vec2>(object.id, object.v2);
        world.add<float>(object.id, Dynamo::Random::random());

        ground_truth.push_back(object);
    }

    unsigned count = 0;
    world.foreach<Dynamo::Vec2>([&count, &ground_truth](Dynamo::ECS::Entity entity, Dynamo::Vec2 &v2) {
        uintptr_t i = reinterpret_cast<uintptr_t>(entity);
        REQUIRE(ground_truth[i].id == entity);
        REQUIRE(ground_truth[i].v2 == v2);
        count++;
    });
    REQUIRE(count == 100);
}

TEST_CASE("ECS::World foreach group", "[ECS::World]") {
    Dynamo::ECS::World world;

    struct Object {
        Dynamo::ECS::Entity id;
        Dynamo::Vec2 v2;
        Dynamo::Vec3 v3;
        float f;
    };
    std::vector<Object> ground_truth;

    for (unsigned i = 0; i < 200; i++) {
        Object object;
        object.id = world.create();
        object.v2 = {
            Dynamo::Random::random(),
            Dynamo::Random::random(),
        };
        object.v3 = {
            Dynamo::Random::random(),
            Dynamo::Random::random(),
        };
        object.f = Dynamo::Random::random();

        if (i % 2 == 0) {
            world.add<Dynamo::Vec2>(object.id, object.v2);
            world.add<Dynamo::Vec3>(object.id, object.v3);
        }
        world.add<float>(object.id, object.f);

        ground_truth.emplace_back(object);
    }

    unsigned count = 0;
    world.foreach_group<Dynamo::Vec2, Dynamo::Vec3, float>(
        [&count, &ground_truth](Dynamo::ECS::Entity entity, Dynamo::Vec2 &v2, Dynamo::Vec3 &v3, float &f) {
            uintptr_t i = reinterpret_cast<uintptr_t>(entity);
            REQUIRE(ground_truth[i].id == entity);
            REQUIRE(ground_truth[i].v2 == v2);
            REQUIRE(ground_truth[i].v3 == v3);
            REQUIRE(ground_truth[i].f == f);
            count++;
        });
    REQUIRE(count == 100);

    // Subset
    count = 0;
    world.foreach_group<Dynamo::Vec2, float>(
        [&count, &ground_truth](Dynamo::ECS::Entity entity, Dynamo::Vec2 &v2, float &f) {
            uintptr_t i = reinterpret_cast<uintptr_t>(entity);
            REQUIRE(ground_truth[i].v2 == v2);
            REQUIRE(ground_truth[i].f == f);
            count++;
        });
    REQUIRE(count == 100);

    // Individual
    count = 0;
    world.foreach_group<float>([&count, &ground_truth](Dynamo::ECS::Entity entity, float &f) {
        uintptr_t i = reinterpret_cast<uintptr_t>(entity);
        REQUIRE(ground_truth[i].f == f);
        count++;
    });
    REQUIRE(count == 200);

    // Group Exclusion
    count = 0;
    Dynamo::ECS::Group<Dynamo::Vec3, Dynamo::Vec2> exclude_group;
    world.foreach_group<float>(
        [&count, &ground_truth](Dynamo::ECS::Entity entity, float &f) {
            uintptr_t i = reinterpret_cast<uintptr_t>(entity);
            REQUIRE(ground_truth[i].f == f);
            count++;
        },
        exclude_group);
    REQUIRE(count == 100);
}
