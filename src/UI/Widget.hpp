#pragma once

#include <Graphics/Mesh.hpp>
#include <Graphics/Pipeline.hpp>

namespace Dynamo::UI {
    /**
     * @brief Widget types.
     *
     */
    enum class WidgetType {
        Container,
        MaxWidgetType,
    };

    /**
     * @brief Widget material and associated settings uniform.
     *
     */
    struct WidgetMaterial {
        /**
         * @brief Graphics pipeline.
         *
         */
        Graphics::Pipeline pipeline;

        /**
         * @brief Settings uniform.
         *
         */
        Graphics::Uniform uniform;
    };

    /**
     * @brief Widget object.
     *
     */
    struct Widget {
        /**
         * @brief Widget type.
         *
         */
        WidgetType type;

        /**
         * @brief Mesh handle.
         *
         */
        Graphics::Mesh mesh;

        /**
         * @brief Widget material.
         *
         */
        WidgetMaterial material;
    };
} // namespace Dynamo::UI