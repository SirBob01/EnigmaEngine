#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Graphics::Vulkan {
    // We only have 4096 guaranteed allocations. 32M * 4096 is over 100GB, so this should be enough.
    constexpr VkDeviceSize MIN_ALLOCATION_SIZE = 32 * (1 << 20);

    MemoryPool::MemoryPool(VkDevice device, const PhysicalDevice &physical) :
        _device(device),
        _physical(physical),
        _groups(physical.memory.memoryTypeCount) {}

    MemoryPool::~MemoryPool() {
        // Free device memory
        for (const MemoryGroup &group : _groups) {
            for (const Memory &memory : group) {
                vkFreeMemory(_device, memory.handle, nullptr);
            }
        }
        _groups.clear();
    }

    unsigned MemoryPool::find_type_index(const VkMemoryRequirements &requirements,
                                         VkMemoryPropertyFlags properties) const {
        unsigned type_index = 0;
        while (type_index < _physical.memory.memoryTypeCount) {
            VkMemoryType type = _physical.memory.memoryTypes[type_index];
            bool has_type = requirements.memoryTypeBits & (1 << type_index);
            bool has_properties = (properties & type.propertyFlags) == properties;
            if (has_type && has_properties) {
                break;
            }
            type_index++;
        }
        DYN_ASSERT(type_index < _groups.size());
        return type_index;
    }

    MemoryPool::VirtualMemory MemoryPool::allocate_memory(const VkMemoryRequirements &requirements,
                                                          VkMemoryPropertyFlags properties) {
        // Find compatible memory block and suballocate
        unsigned type = find_type_index(requirements, properties);
        MemoryGroup &group = _groups[type];
        for (unsigned index = 0; index < group.size(); index++) {
            Memory &memory = group[index];
            char *base_ptr = static_cast<char *>(memory.mapped);

            std::optional<unsigned> result = memory.allocator.reserve(requirements.size, requirements.alignment);
            if (result.has_value()) {
                VirtualMemory allocation;
                allocation.key.type = type;
                allocation.key.index = index;
                allocation.key.offset = result.value();
                allocation.memory = memory.handle;
                allocation.mapped = nullptr;
                if (base_ptr) {
                    allocation.mapped = base_ptr + allocation.key.offset;
                }
                return allocation;
            }
        }

        // None found, build new memory block and suballocate
        VkDeviceSize heap_size = std::max(requirements.size, MIN_ALLOCATION_SIZE);
        VkDeviceMemory memory = VkDeviceMemory_allocate(_device, type, heap_size);
        void *mapped = nullptr;
        if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            VkResult_check("Map Memory", vkMapMemory(_device, memory, 0, heap_size, 0, &mapped));
        }
        group.push_back({memory, heap_size, mapped});

        VirtualMemory allocation;
        allocation.key.type = type;
        allocation.key.index = group.size() - 1;
        allocation.key.offset = group.back().allocator.reserve(requirements.size, requirements.alignment).value();
        allocation.memory = memory;
        allocation.mapped = mapped;
        return allocation;
    }

    VirtualBuffer MemoryPool::build(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, unsigned size) {
        // Create the buffer
        VkBuffer buffer = VkBuffer_create(_device, usage, size, nullptr, 0);

        // Allocate memory and bind to buffer
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(_device, buffer, &requirements);

        VirtualMemory memory = allocate_memory(requirements, properties);
        vkBindBufferMemory(_device, buffer, memory.memory, memory.key.offset);

        VirtualBuffer v_buffer;
        v_buffer.buffer = buffer;
        v_buffer.key = memory.key;
        v_buffer.offset = 0; // TODO: Suballocation
        v_buffer.mapped = memory.mapped;
        return v_buffer;
    }

    VirtualImage MemoryPool::build(const VkExtent3D &extent,
                                   VkFormat format,
                                   VkImageLayout layout,
                                   VkImageType type,
                                   VkImageTiling tiling,
                                   VkImageUsageFlags usage,
                                   VkSampleCountFlagBits samples,
                                   VkImageCreateFlags flags,
                                   unsigned mip_levels,
                                   unsigned array_layers) {
        // Create the image
        VkImage image = VkImage_create(_device,
                                       extent,
                                       format,
                                       layout,
                                       type,
                                       tiling,
                                       usage,
                                       samples,
                                       flags,
                                       mip_levels,
                                       array_layers,
                                       nullptr,
                                       0);

        // Allocate memory and bind to image
        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(_device, image, &requirements);

        VirtualMemory memory = allocate_memory(requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindImageMemory(_device, image, memory.memory, memory.key.offset);

        VirtualImage v_image;
        v_image.image = image;
        v_image.key = memory.key;
        v_image.mapped = memory.mapped;
        return v_image;
    }

    void MemoryPool::free(const VirtualBuffer &allocation) {
        vkDestroyBuffer(_device, allocation.buffer, nullptr);
        Memory &memory = _groups[allocation.key.type][allocation.key.index];
        memory.allocator.free(allocation.key.offset);
    }

    void MemoryPool::free(const VirtualImage &allocation) {
        vkDestroyImage(_device, allocation.image, nullptr);
        Memory &memory = _groups[allocation.key.type][allocation.key.index];
        memory.allocator.free(allocation.key.offset);
    }
} // namespace Dynamo::Graphics::Vulkan