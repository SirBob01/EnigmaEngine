#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Buffer.hpp>
#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct BufferInstance {
        VkBuffer buffer;
        VirtualMemory memory;
        unsigned offset;
    };

    class BufferRegistry {
        const Context &_context;
        MemoryPool &_memory;

        SparseArray<Buffer, BufferInstance> _instances;

      public:
        BufferRegistry(const Context &context, MemoryPool &memory);
        ~BufferRegistry();

        Buffer build(const BufferDescriptor &descriptor);

        const BufferInstance &get(Buffer buffer) const;

        void destroy(Buffer buffer);
    };
} // namespace Dynamo::Graphics::Vulkan