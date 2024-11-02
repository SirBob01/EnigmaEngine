#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../Common.hpp"

TEST_CASE("Camera perspective", "[Camera]") {
    float w = 640;
    float h = 480;
    Dynamo::Camera camera(Dynamo::to_radians(45), w / h, 0.1, 10);

    REQUIRE_THAT(camera.projection.values[0], Approx(1.810660));
    REQUIRE(camera.projection.values[1] == 0);
    REQUIRE(camera.projection.values[2] == 0);
    REQUIRE(camera.projection.values[3] == 0);

    REQUIRE(camera.projection.values[4] == 0);
    REQUIRE_THAT(camera.projection.values[5], Approx(-2.414213));
    REQUIRE(camera.projection.values[6] == 0);
    REQUIRE(camera.projection.values[7] == 0);

    REQUIRE(camera.projection.values[8] == 0);
    REQUIRE(camera.projection.values[9] == 0);
    REQUIRE_THAT(camera.projection.values[10], Approx(-1.020202));
    REQUIRE(camera.projection.values[11] == -1);

    REQUIRE(camera.projection.values[12] == 0);
    REQUIRE(camera.projection.values[13] == 0);
    REQUIRE_THAT(camera.projection.values[14], Approx(-0.202020));
    REQUIRE(camera.projection.values[15] == 0);
}

TEST_CASE("Camera view", "[Camera]") {
    float w = 640;
    float h = 480;
    Dynamo::Camera camera(Dynamo::to_radians(45), w / h, 0.1, 10);
    camera.look_at({2, 2, 2}, {0, 0, 0}, {0, 0, 1});

    REQUIRE_THAT(camera.view.values[0], Approx(-0.707107));
    REQUIRE_THAT(camera.view.values[1], Approx(-0.408248));
    REQUIRE_THAT(camera.view.values[2], Approx(0.577350));
    REQUIRE(camera.view.values[3] == 0);
    REQUIRE_THAT(camera.view.values[4], Approx(0.707107));
    REQUIRE_THAT(camera.view.values[5], Approx(-0.408248));
    REQUIRE_THAT(camera.view.values[6], Approx(0.577350));
    REQUIRE(camera.view.values[7] == 0);
    REQUIRE(camera.view.values[8] == 0);
    REQUIRE_THAT(camera.view.values[9], Approx(0.816497));
    REQUIRE_THAT(camera.view.values[10], Approx(0.577350));
    REQUIRE(camera.view.values[11] == 0);
    REQUIRE(camera.view.values[12] == 0);
    REQUIRE(camera.view.values[13] == 0);
    REQUIRE_THAT(camera.view.values[14], Approx(-3.464102));
    REQUIRE(camera.view.values[15] == 1);
}
