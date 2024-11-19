#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/Context.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/Allocator.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct AllocationKey {
        unsigned offset;
        unsigned type;
        unsigned index;
    };

    struct VirtualMemory {
        VkDeviceMemory memory;
        AllocationKey key;
        void *mapped;
    };

    class MemoryPool {
        struct Memory {
            VkDeviceMemory memory;
            Allocator allocator;
            void *mapped;
        };
        using MemoryGroup = std::vector<Memory>;

        const Context &_context;
        std::vector<MemoryGroup> _groups;

        unsigned find_type_index(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties) const;

      public:
        MemoryPool(const Context &context);
        ~MemoryPool();

        VirtualMemory allocate(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties);

        void free(const VirtualMemory &allocation);
    };
}; // namespace Dynamo::Graphics::Vulkan