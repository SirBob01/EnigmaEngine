#pragma once

#include <Math/Vec2.hpp>

namespace Dynamo::UI {
    static const Vec2 QUAD_VERTICES[4] = {
        Dynamo::Vec2(0, 0),
        Dynamo::Vec2(1, 0),
        Dynamo::Vec2(1, 1),
        Dynamo::Vec2(0, 1),
    };

    static const unsigned QUAD_INDICES[6] = {
        2,
        1,
        0,
        0,
        3,
        2,
    };

    const std::string CONTAINER_VERTEX_SHADER = R"(
    #version 450
    layout(location = 0) in vec2 position;

    layout(set = 0, binding = 0) uniform ContainerUniform {
        vec2 viewport;
        vec2 offset;
        vec2 scale;
        vec4 color;
        float radius;
    } settings;

    layout(location = 0) out vec4 color;
    layout(location = 1) out vec2 scale;
    layout(location = 2) out vec2 uv;
    layout(location = 3) out float radius;

    void main() {
        vec2 screen_space = position * settings.scale + settings.offset;
        vec2 clip_space = (screen_space / settings.viewport) * 2 - 1;
        gl_Position = vec4(clip_space, 0.0, 1.0);

        color = settings.color;
        scale = settings.scale;
        uv = position;
        radius = settings.radius;
    }
    )";

    const std::string CONTAINER_FRAGMENT_SHADER = R"(
    #version 450
    layout(location = 0) in vec4 color;
    layout(location = 1) in vec2 scale;
    layout(location = 2) in vec2 uv;
    layout(location = 3) flat in float radius;

    layout(location = 0) out vec4 frag;

    float rect_distance() {
        vec2 extents = scale * 0.5;
        vec2 coords = abs(2 * (uv - vec2(0.5))) * (extents + radius);
        vec2 delta = max(coords - extents, 0);
        return length(delta);
    }

    void main() {
        if (rect_distance() > radius) {
            discard;
        }
        frag = color;
    }
    )";
} // namespace Dynamo::UI