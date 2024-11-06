#pragma once

#include <Graphics/Material.hpp>
#include <Graphics/Mesh.hpp>

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
         * @brief Model material properties.
         *
         */
        Material material;

        /**
         * @brief Group index used in sorting.
         *
         */
        int group = 0;
    };
} // namespace Dynamo::Graphics