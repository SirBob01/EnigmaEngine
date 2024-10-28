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
        VkBuffer index_buffer;
        unsigned index_offset;
        unsigned index_count;
        unsigned vertex_count;
        unsigned instance_count;
        VkIndexType index_type;
    };

    /**
     * @brief Mesh registry.
     *
     */
    class MeshRegistry {
        VkDevice _device;
        Buffer _vertex;
        Buffer _index;
        Buffer _staging;

        SparseArray<Mesh, MeshAllocation> _allocations;

      public:
        MeshRegistry(VkDevice device, const PhysicalDevice &physical, VkCommandPool transfer_pool);
        MeshRegistry() = default;

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
         * @return Mesh
         */
        Mesh build(const MeshDescriptor &descriptor);

        /**
         * @brief Free all allocated buffers for a mesh.
         *
         * @param mesh
         */
        void destroy(Mesh mesh);

        /**
         * @brief Destroy mesh allocation buffers.
         *
         */
        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan