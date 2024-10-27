#pragma once

#include <string>

#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Shader handle.
     *
     */
    DYN_DEFINE_ID_TYPE(Shader);

    /**
     * @brief Shader pipeline stage.
     *
     */
    enum class ShaderStage {
        Vertex,
        Fragment,
        Compute,
        TesselationControl,
        TesselationEvaluation,
    };

    /**
     * @brief Shader descriptor.
     *
     */
    struct ShaderDescriptor {
        /**
         * @brief Name of the shader.
         *
         */
        std::string name;

        /**
         * @brief Shader source code in the native language of the renderer backend.
         *
         * Source must have a `void main()` entry function.
         *
         */
        std::string code;

        /**
         * @brief Shader stage.
         *
         */
        ShaderStage stage;
    };
} // namespace Dynamo::Graphics