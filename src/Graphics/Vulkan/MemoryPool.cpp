#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Graphics::Vulkan {
    MemoryPool::MemoryPool(VkDevice device, const PhysicalDevice &physical) :
        _device(device), _physical(&physical), _groups(physical.memory.memoryTypeCount) {}

    unsigned MemoryPool::find_type_index(const VkMemoryRequirements &requirements,
                                         VkMemoryPropertyFlags properties) const {
        unsigned type_index = 0;
        while (type_index < _physical->memory.memoryTypeCount) {
            VkMemoryType type = _physical->memory.memoryTypes[type_index];
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
        VirtualMemory allocation;
        allocation.key.type = find_type_index(requirements, properties);
        allocation.mapped = nullptr;

        MemoryGroup &group = _groups[allocation.key.type];
        for (allocation.key.index = 0; allocation.key.index < group.size(); allocation.key.index++) {
            Memory &memory = group[allocation.key.index];

            std::optional<unsigned> result = memory.allocator.reserve(requirements.size, requirements.alignment);
            if (result.has_value()) {
                allocation.memory = memory.handle;
                allocation.key.offset = result.value();
                if (memory.mapped) {
                    allocation.mapped = static_cast<char *>(memory.mapped) + allocation.key.offset;
                }
            }
        }

        // None found, allocate new memory
        VkDeviceSize heap_size = std::max(requirements.size, MEMORY_ALLOCATION_SIZE);
        allocation.memory = VkDeviceMemory_allocate(_device, allocation.key.type, heap_size);
        if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            VkResult_check("Map Memory", vkMapMemory(_device, allocation.memory, 0, heap_size, 0, &allocation.mapped));
        }
        group.push_back({allocation.memory, heap_size, allocation.mapped});

        Memory &memory = group.back();
        allocation.key.offset = memory.allocator.reserve(requirements.size, requirements.alignment).value();
        return allocation;
    }

    VirtualBuffer MemoryPool::build(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, unsigned size) {
        // Create the buffer
        VirtualBuffer buffer;
        buffer.buffer = VkBuffer_create(_device, usage, size, nullptr, 0);

        // Allocate memory and bind to buffer
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(_device, buffer.buffer, &requirements);

        VirtualMemory memory = allocate_memory(requirements, properties);
        buffer.key = memory.key;
        buffer.offset = 0; // TODO: Suballocation
        buffer.mapped = memory.mapped;

        vkBindBufferMemory(_device, buffer.buffer, memory.memory, buffer.offset);
        return buffer;
    }

    VirtualImage MemoryPool::build(const TextureDescriptor &descriptor) {
        // Create the image
        VkExtent3D extent;
        extent.width = descriptor.width;
        extent.height = descriptor.height;
        extent.depth = 1;

        VkFormat format = convert_texture_format(descriptor.format);
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        VirtualImage image;
        image.image = VkImage_create(_device,
                                     extent,
                                     format,
                                     VK_IMAGE_LAYOUT_UNDEFINED,
                                     VK_IMAGE_TYPE_2D,
                                     VK_IMAGE_TILING_OPTIMAL,
                                     usage,
                                     VK_SAMPLE_COUNT_1_BIT,
                                     1,
                                     1,
                                     nullptr,
                                     0);

        // Allocate memory and bind to image
        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(_device, image.image, &requirements);

        VirtualMemory memory = allocate_memory(requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        image.key = memory.key;
        image.mapped = memory.mapped;

        vkBindImageMemory(_device, image.image, memory.memory, image.key.offset);
        return image;
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

    void MemoryPool::destroy() {
        // Free device memory
        for (const MemoryGroup &group : _groups) {
            for (const Memory &memory : group) {
                vkFreeMemory(_device, memory.handle, nullptr);
            }
        }
        _groups.clear();
    }
} // namespace Dynamo::Graphics::Vulkan