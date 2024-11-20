#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Buffer.hpp>
#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct BufferInstance {
        VkBuffer buffer;
        unsigned main_group;
        unsigned main_index;
        unsigned offset;
        void *mapped;
    };

    struct MainBuffer {
        VkBuffer buffer;
        Allocator allocator;
        Allocation allocation;
        void *mapped;
    };

    class BufferRegistry {
        const Context &_context;
        MemoryPool &_memory;

        std::vector<std::vector<MainBuffer>> _groups;
        SparseArray<Buffer, BufferInstance> _instances;

        MainBuffer build_main(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

        unsigned find_type_index(BufferUsage usage, MemoryProperty properties);

      public:
        BufferRegistry(const Context &context, MemoryPool &memory);
        ~BufferRegistry();

        Buffer build(const BufferDescriptor &descriptor);

        const BufferInstance &get(Buffer buffer) const;

        void destroy(Buffer buffer);
    };
} // namespace Dynamo::Graphics::Vulkan