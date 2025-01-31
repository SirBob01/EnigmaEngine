#pragma once

#include <string>
#include <vector>

#include <Graphics/Shader.hpp>
#include <Math/Color.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Pipeline handle.
     *
     */
    DYN_DEFINE_ID_TYPE(Pipeline);

    /**
     * @brief Uniform variable handle.
     *
     */
    DYN_DEFINE_ID_TYPE(Uniform);

    /**
     * @brief Uniform variable group handle.
     *
     */
    DYN_DEFINE_ID_TYPE(UniformGroup);

    /**
     * @brief Topology modes.
     *
     */
    enum class Topology : uint8_t {
        Point,
        Triangle,
        Line,
    };

    /**
     * @brief Fill modes.
     *
     */
    enum class Fill : uint8_t {
        Point,
        Line,
        Face,
    };

    /**
     * @brief Cull modes.
     *
     */
    enum class Cull : uint8_t {
        None,
        Back,
        Front,
    };

    /**
     * @brief Comparison operators.
     *
     */
    enum class CompareOp : uint8_t {
        Always,
        Never,
        Equal,
        NotEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
    };

    /**
     * @brief Pipeline descriptor.
     *
     */
    struct PipelineDescriptor {
        /**
         * @brief Vertex shader handle.
         *
         */
        Shader vertex;

        /**
         * @brief Fragment shader handle.
         *
         */
        Shader fragment;

        /**
         * @brief Topology of the vertex input.
         *
         */
        Topology topology = Topology::Triangle;

        /**
         * @brief Fill mode on draw.
         *
         */
        Fill fill = Fill::Face;

        /**
         * @brief Cull mode.
         *
         */
        Cull cull = Cull::Back;

        /**
         * @brief RGBA color write mask.
         *
         */
        Color color_mask = Color(1, 1, 1, 1);

        /**
         * @brief Perform depth test to discard fragments.
         *
         */
        bool depth_test = true;

        /**
         * @brief Write to the depth buffer (if depth test is enabled).
         *
         */
        bool depth_write = true;

        /**
         * @brief Depth test operation.
         *
         */
        CompareOp depth_test_op = CompareOp::LessEqual;

        /**
         * @brief Names of uniforms that are shared across shader invocations (global variable).
         *
         */
        std::vector<std::string> shared_uniforms;
    };
} // namespace Dynamo::Graphics
