#pragma once

#include <Graphics/Mesh.hpp>
#include <Graphics/Pipeline.hpp>

namespace Dynamo::UI {
    enum class WidgetType {
        Container,
        MaxWidgetType,
    };

    struct WidgetMaterial {
        Graphics::Pipeline pipeline;
        Graphics::Uniform uniform;
    };

    struct Widget {
        WidgetType type;
        Graphics::Mesh mesh;
        WidgetMaterial material;
    };
} // namespace Dynamo::UI