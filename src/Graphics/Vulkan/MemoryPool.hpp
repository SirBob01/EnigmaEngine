#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/Allocator.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct AllocationKey {
        unsigned offset;
        unsigned type;
        unsigned index;
    };

    struct VirtualBuffer {
        VkBuffer buffer;
        AllocationKey key;
        unsigned offset;
        void *mapped;
    };

    struct VirtualImage {
        VkImage image;
        AllocationKey key;
        void *mapped;
    };

    class MemoryPool {
        struct VirtualMemory {
            VkDeviceMemory memory;
            AllocationKey key;
            void *mapped;
        };
        struct Memory {
            VkDeviceMemory handle;
            Allocator allocator;
            void *mapped;
        };
        using MemoryGroup = std::vector<Memory>;

        VkDevice _device;
        const PhysicalDevice &_physical;
        std::vector<MemoryGroup> _groups;

        unsigned find_type_index(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties) const;

        VirtualMemory allocate_memory(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties);

      public:
        MemoryPool(VkDevice device, const PhysicalDevice &physical);
        ~MemoryPool();

        VirtualBuffer build(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, unsigned size);

        VirtualImage build(const VkExtent3D &extent,
                           VkFormat format,
                           VkImageLayout layout,
                           VkImageType type,
                           VkImageTiling tiling,
                           VkImageUsageFlags usage,
                           VkSampleCountFlagBits samples,
                           VkImageCreateFlags flags,
                           unsigned mip_levels,
                           unsigned array_layers);

        void free(const VirtualBuffer &allocation);

        void free(const VirtualImage &allocation);
    };
}; // namespace Dynamo::Graphics::Vulkan