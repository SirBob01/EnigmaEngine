#include <Graphics/Vulkan/Buffer.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    Buffer::Buffer(VkDevice device,
                   const PhysicalDevice &physical,
                   VkCommandBuffer command_buffer,
                   VkBufferUsageFlagBits usage,
                   VkMemoryPropertyFlags properties) :
        _device(device),
        _physical_settings(physical.memory), _command_buffer(command_buffer),
        _usage(usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT), _properties(properties),
        _allocator(MIN_ALLOCATION_SIZE) {
        _handle = VkBuffer_create(_device, _usage, _allocator.capacity(), nullptr, 0);
        _memory = allocate(_handle);
        vkBindBufferMemory(_device, _handle, _memory, 0);
        vkGetDeviceQueue(_device, physical.transfer_queues.index, 0, &_transfer_queue);
    }

    VkDeviceMemory Buffer::allocate(VkBuffer buffer) {
        vkGetBufferMemoryRequirements(_device, buffer, &_requirements);

        unsigned type_index;
        for (type_index = 0; type_index < _physical_settings.memoryTypeCount; type_index++) {
            VkMemoryType type = _physical_settings.memoryTypes[type_index];
            if ((_requirements.memoryTypeBits & (1 << type_index)) &&
                ((_properties & type.propertyFlags) == _properties)) {
                break;
            }
        }
        if (type_index == _physical_settings.memoryTypeCount) {
            Log::error("Vulkan could not find suitable memory type for buffer.");
        }

        VkDeviceMemory memory = VkDeviceMemory_allocate(_device, type_index, _requirements.size);
        if (_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            VkResult_log(
                "Map Device Memory",
                vkMapMemory(_device, memory, 0, _allocator.capacity(), 0, reinterpret_cast<void **>(&_mapped)));
            DYN_ASSERT(_mapped != nullptr);
        } else {
            _mapped = nullptr;
        }
        return memory;
    }

    VkBuffer Buffer::handle() const { return _handle; }

    unsigned Buffer::capacity() const { return _allocator.capacity(); }

    unsigned Buffer::reserve(unsigned size) {
        std::optional<unsigned> result = _allocator.reserve(size, _requirements.alignment);
        if (result.has_value()) {
            return result.value();
        } else {
            unsigned curr = _allocator.capacity();
            unsigned next = std::max(curr + size, curr * 2);
            resize(align_size(next, _requirements.alignment));

            // If this fails, we're in trouble...
            return _allocator.reserve(size, _requirements.alignment).value();
        }
    }

    void Buffer::free(unsigned block_offset) { _allocator.free(block_offset); }

    unsigned Buffer::size(unsigned block_offset) const { return _allocator.size(block_offset); }

    void Buffer::resize(unsigned size) {
        // Do not resize if target is less than the current capacity
        if (size < _allocator.capacity()) return;

        // Allocate new memory and buffer
        VkBuffer next_handle = VkBuffer_create(_device, _usage, size, nullptr, 0);
        VkDeviceMemory next_memory = allocate(next_handle);
        vkBindBufferMemory(_device, next_handle, next_memory, 0);

        // Copy contents to new buffer
        VkBufferCopy region;
        region.dstOffset = 0;
        region.srcOffset = 0;
        region.size = _allocator.capacity();

        VkBuffer_immediate_copy(_handle, next_handle, _transfer_queue, _command_buffer, &region, 1);
        vkQueueWaitIdle(_transfer_queue);

        // Destroy old resources
        vkDestroyBuffer(_device, _handle, nullptr);
        vkFreeMemory(_device, _memory, nullptr);

        // Reassign
        _handle = next_handle;
        _memory = next_memory;

        // Update the allocator
        _allocator.grow(size);
    }

    void *Buffer::get_mapped(unsigned block_offset) {
        DYN_ASSERT(_allocator.is_reserved(block_offset) && _mapped != nullptr);
        return _mapped + block_offset;
    }

    void Buffer::copy_to(Buffer &dst, VkBufferCopy *regions, unsigned region_count) {
        VkBuffer_immediate_copy(_handle, dst._handle, _transfer_queue, _command_buffer, regions, region_count);
        vkQueueWaitIdle(_transfer_queue);
    }

    void Buffer::destroy() {
        vkDestroyBuffer(_device, _handle, nullptr);
        vkFreeMemory(_device, _memory, nullptr);
    }
} // namespace Dynamo::Graphics::Vulkan