#include <Dynamo.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../Common.hpp"

float w = 640;
float h = 480;

Dynamo::Perspective perspective{Dynamo::to_radians(45), w / h, 0.1, 10};
Dynamo::Orthographic orthographic{{{-1, -1}, {1, 1}}, 0.1, 10};

TEST_CASE("Camera perspective", "[Camera]") {
    Dynamo::Camera camera(perspective);

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

TEST_CASE("Camera orthographic", "[Camera]") {
    Dynamo::Camera camera(orthographic);

    REQUIRE(camera.projection.values[0] == 1.000000);
    REQUIRE(camera.projection.values[1] == 0.000000);
    REQUIRE(camera.projection.values[2] == 0.000000);
    REQUIRE(camera.projection.values[3] == 0.000000);
    REQUIRE(camera.projection.values[4] == 0.000000);
    REQUIRE(camera.projection.values[5] == -1.000000);
    REQUIRE(camera.projection.values[6] == 0.000000);
    REQUIRE(camera.projection.values[7] == 0.000000);
    REQUIRE(camera.projection.values[8] == 0.000000);
    REQUIRE(camera.projection.values[9] == 0.000000);
    REQUIRE_THAT(camera.projection.values[10], Approx(-0.101010));
    REQUIRE(camera.projection.values[11] == 0.000000);
    REQUIRE(camera.projection.values[12] == -0.000000);
    REQUIRE(camera.projection.values[13] == 0.000000);
    REQUIRE_THAT(camera.projection.values[14], Approx(-0.010101));
    REQUIRE(camera.projection.values[15] == 1.000000);
}

TEST_CASE("Camera make perspective", "[Camera]") {
    Dynamo::Camera camera;
    camera.make(perspective);

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

TEST_CASE("Camera make orthographic", "[Camera]") {
    Dynamo::Camera camera;
    camera.make(orthographic);

    REQUIRE(camera.projection.values[0] == 1.000000);
    REQUIRE(camera.projection.values[1] == 0.000000);
    REQUIRE(camera.projection.values[2] == 0.000000);
    REQUIRE(camera.projection.values[3] == 0.000000);
    REQUIRE(camera.projection.values[4] == 0.000000);
    REQUIRE(camera.projection.values[5] == -1.000000);
    REQUIRE(camera.projection.values[6] == 0.000000);
    REQUIRE(camera.projection.values[7] == 0.000000);
    REQUIRE(camera.projection.values[8] == 0.000000);
    REQUIRE(camera.projection.values[9] == 0.000000);
    REQUIRE_THAT(camera.projection.values[10], Approx(-0.101010));
    REQUIRE(camera.projection.values[11] == 0.000000);
    REQUIRE(camera.projection.values[12] == -0.000000);
    REQUIRE(camera.projection.values[13] == 0.000000);
    REQUIRE_THAT(camera.projection.values[14], Approx(-0.010101));
    REQUIRE(camera.projection.values[15] == 1.000000);
}

TEST_CASE("Camera orient", "[Camera]") {
    Dynamo::Camera camera;
    Dynamo::Vec3 forward(-2, -2, -2);
    forward.normalize();

    camera.orient({2, 2, 2}, forward, {0, 0, 1});

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

TEST_CASE("Camera look at", "[Camera]") {
    Dynamo::Camera camera;
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
