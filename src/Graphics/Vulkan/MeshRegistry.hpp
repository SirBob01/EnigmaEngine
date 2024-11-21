#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

#include <Graphics/Mesh.hpp>
#include <Graphics/Vulkan/BufferRegistry.hpp>
#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct MeshInstance {
        std::vector<VkBuffer> attribute_buffers;
        std::vector<VkDeviceSize> attribute_offsets;
        VkBuffer index_buffer;
        VkDeviceSize index_offset;
        VkIndexType index_type;
        unsigned index_count;
        unsigned vertex_count;
        unsigned instance_count;
    };

    class MeshRegistry {
        const BufferRegistry &_buffers;

        SparseArray<Mesh, MeshInstance> _instances;

      public:
        MeshRegistry(const BufferRegistry &buffers);
        ~MeshRegistry();

        Mesh build(const MeshDescriptor &descriptor);

        MeshInstance &get(Mesh mesh);

        void destroy(Mesh mesh);
    };
} // namespace Dynamo::Graphics::Vulkan