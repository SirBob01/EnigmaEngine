#pragma once

#include <vector>

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
    enum class IndexType {
        None,
        U16,
        U32,
    };

    /**
     * @brief Mesh descriptor.
     *
     * Attribute order must match how they are declared in the vertex shader.
     *
     */
    struct MeshDescriptor {
        using AttributeBuffer = std::vector<unsigned char>;

        std::vector<AttributeBuffer> attributes;
        unsigned vertex_count;
        unsigned instance_count;

        std::vector<unsigned> indices;
        IndexType index_type;

        /**
         * @brief Construct a Mesh Descriptor.
         *
         * @param vertex_count
         * @param index_type
         */
        MeshDescriptor(unsigned vertex_count, IndexType index_type) :
            vertex_count(vertex_count),
            instance_count(1),
            index_type(index_type) {}

        /**
         * @brief Construct an instanced Mesh Descriptor.
         *
         * @param vertex_count
         * @param instance_count
         * @param index_type
         */
        MeshDescriptor(unsigned vertex_count, unsigned instance_count, IndexType index_type) :
            vertex_count(vertex_count),
            instance_count(instance_count),
            index_type(index_type) {}

        /**
         * @brief Add a vertex attribute to the mesh.
         *
         * @tparam T
         * @param array
         * @param count
         */
        template <typename T>
        void add_vertex_attribute(const T *array) {
            attributes.emplace_back();
            AttributeBuffer &buffer = attributes.back();
            buffer.resize(vertex_count * sizeof(T));
            std::copy(array, array + vertex_count, reinterpret_cast<T *>(buffer.data()));
        }

        /**
         * @brief Add an instance attribute to the mesh.
         *
         * Attribute names should be prefixed with "instance_" in the relevant shader modules.
         *
         * @tparam T
         * @param array
         * @param count
         */
        template <typename T>
        void add_instance_attribute(const T *array) {
            attributes.emplace_back();
            AttributeBuffer &buffer = attributes.back();
            buffer.resize(instance_count * sizeof(T));
            std::copy(array, array + instance_count, reinterpret_cast<T *>(buffer.data()));
        }
    };
} // namespace Dynamo::Graphics
