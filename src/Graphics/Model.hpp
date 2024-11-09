#pragma once

#include <Graphics/Mesh.hpp>
#include <Graphics/Pipeline.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief 3D Model.
     *
     */
    struct Model {
        /**
         * @brief Mesh geometry handle.
         *
         */
        Mesh mesh;

        /**
         * @brief Model pipeline.
         *
         */
        Pipeline pipeline;

        /**
         * @brief Group index used in sorting.
         *
         */
        int group = 0;
    };
} // namespace Dynamo::Graphics