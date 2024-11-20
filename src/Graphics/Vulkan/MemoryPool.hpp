#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/Context.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/Allocator.hpp>

namespace Dynamo::Graphics::Vulkan {
    // We only have 4096 guaranteed allocations. 32M * 4096 is over 100GB, so this should be enough.
    constexpr VkDeviceSize MIN_ALLOCATION_SIZE = 32 * (1 << 20);

    // Allocation key
    struct Allocation {
        unsigned offset;
        unsigned type;
        unsigned index;
    };

    // Suballocated block of memory
    struct SubMemory {
        VkDeviceMemory memory;
        Allocation allocation;
        void *mapped;
    };

    // Block of allocated device memory
    struct MainMemory {
        VkDeviceMemory memory;
        Allocator allocator;
        void *mapped;
    };

    class MemoryPool {
        const Context &_context;
        std::vector<std::vector<MainMemory>> _groups;

        unsigned find_type_index(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties) const;

      public:
        MemoryPool(const Context &context);
        ~MemoryPool();

        SubMemory allocate(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties);

        void free(const Allocation &allocation);
    };
}; // namespace Dynamo::Graphics::Vulkan