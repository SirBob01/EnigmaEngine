#pragma once

#include <vector>

#include <Graphics/Buffer.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Mesh handle.
     *
     */
    DYN_DEFINE_ID_TYPE(Mesh);

    /**
     * @brief Mesh index integer width.
     *
     */
    enum class IndexType : uint8_t {
        None,
        U8,
        U16,
        U32,
    };

    /**
     * @brief Vertex attribute.
     *
     */
    struct VertexAttribute {
        /**
         * @brief Device buffer.
         *
         */
        Buffer buffer;

        /**
         * @brief Offset into the buffer.
         *
         */
        unsigned offset;
    };

    /**
     * @brief Mesh descriptor.
     *
     * Attribute order must match how they are declared in the vertex shader.
     *
     */
    struct MeshDescriptor {
        /**
         * @brief Vertex attribute buffers.
         *
         */
        std::vector<VertexAttribute> attributes;

        /**
         * @brief Index buffer.
         *
         */
        VertexAttribute indices;

        /**
         * @brief Index buffer type.
         *
         */
        IndexType index_type;

        /**
         * @brief Number of vertices.
         *
         */
        unsigned vertex_count;

        /**
         * @brief Number of instances.
         *
         */
        unsigned instance_count;

        /**
         * @brief Number of indices.
         *
         */
        unsigned index_count;
    };
} // namespace Dynamo::Graphics
