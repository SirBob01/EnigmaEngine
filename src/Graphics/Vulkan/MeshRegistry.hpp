#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

#include <Graphics/Mesh.hpp>
#include <Graphics/Vulkan/BufferRegistry.hpp>
#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct MeshInstance {
        std::vector<VkDeviceSize> attribute_offsets;
        std::vector<Buffer> virtual_buffers;
        std::vector<VkBuffer> buffers;
        VkBuffer index_buffer;
        unsigned index_offset;
        unsigned index_count;
        unsigned vertex_count;
        unsigned instance_count;
        VkIndexType index_type;
    };

    class MeshRegistry {
        const Context &_context;
        BufferRegistry &_buffers;

        SparseArray<Mesh, MeshInstance> _instances;

        void write_vertices(const void *src, Buffer dst, unsigned size);

      public:
        MeshRegistry(const Context &context, BufferRegistry &buffers);
        ~MeshRegistry();

        MeshInstance &get(Mesh mesh);

        Mesh build(const MeshDescriptor &descriptor);

        void destroy(Mesh mesh);
    };
} // namespace Dynamo::Graphics::Vulkan