#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

#include <Graphics/Mesh.hpp>
#include <Graphics/Vulkan/Buffer.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    /**
     * @brief Mesh GPU allocation instance.
     *
     */
    struct MeshAllocation {
        std::vector<VkDeviceSize> attribute_offsets;
        std::vector<VkBuffer> buffers;
        unsigned index_offset;
        unsigned index_count;
        unsigned vertex_count;
        unsigned instance_count;
        VkIndexType index_type;
    };

    /**
     * @brief Mesh set.
     *
     */
    class MeshSet {
        SparseArray<Mesh, MeshAllocation> _allocations;

      public:
        /**
         * @brief Get a mesh allocation.
         *
         * @param mesh
         * @return MeshAllocation&
         */
        MeshAllocation &get(Mesh mesh);

        /**
         * @brief Upload a mesh descriptor to VRAM.
         *
         * @param descriptor
         * @param vertex_buffer
         * @param index_buffer
         * @param staging_buffer
         * @return Mesh
         */
        Mesh
        build(const MeshDescriptor &descriptor, Buffer &vertex_buffer, Buffer &index_buffer, Buffer &staging_buffer);

        /**
         * @brief Free all allocated buffers for a mesh.
         *
         * @param mesh
         * @param vertex_buffer
         * @param index_buffer
         */
        void free(Mesh mesh, Buffer &vertex_buffer, Buffer &index_buffer);
    };
} // namespace Dynamo::Graphics::Vulkan