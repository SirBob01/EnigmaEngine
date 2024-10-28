#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/Allocator.hpp>

namespace Dynamo::Graphics::Vulkan {
    /**
     * @brief 256M minimum buffer allocation size.
     *
     */
    constexpr unsigned MIN_ALLOCATION_SIZE = 256 * (1 << 20);

    /**
     * @brief Dynamic Vulkan buffer.
     *
     */
    class Buffer {
        VkDevice _device;
        VkPhysicalDeviceMemoryProperties _physical_settings;
        VkCommandBuffer _command_buffer;
        VkQueue _transfer_queue;

        VkBufferUsageFlags _usage;
        VkMemoryPropertyFlags _properties;
        VkMemoryRequirements _requirements;

        Allocator _allocator;

        VkDeviceMemory _memory;
        VkBuffer _handle;

        char *_mapped;

        /**
         * @brief Allocate memory for a buffer.
         *
         * @param buffer
         * @return VkDeviceMemory
         */
        VkDeviceMemory allocate(VkBuffer buffer);

      public:
        Buffer(VkDevice device,
               const PhysicalDevice &physical,
               VkCommandBuffer command_buffer,
               VkBufferUsageFlagBits usage,
               VkMemoryPropertyFlags properties);
        Buffer() = default;

        /**
         * @brief Get the buffer handle.
         *
         * @return VkBuffer
         */
        VkBuffer handle() const;

        /**
         * @brief Get the capacity of the buffer.
         *
         * @return unsigned
         */
        unsigned capacity() const;

        /**
         * @brief Reserve a block of memory.
         *
         * @param size
         * @param alignment
         * @return unsigned
         */
        unsigned reserve(unsigned size);

        /**
         * @brief Free an allocated block.
         *
         * @param block_offset
         * @return unsigned
         */
        void free(unsigned block_offset);

        /**
         * @brief Get the size of an allocated block.
         *
         * @param offset
         * @return unsigned
         */
        unsigned size(unsigned block_offset) const;

        /**
         * @brief Resize the buffer.
         *
         * @param size
         */
        void resize(unsigned size);

        /**
         * @brief Write to the buffer.
         *
         * @param src
         * @param block_offset
         * @param length
         */
        void host_write(const void *src, unsigned block_offset, unsigned length);

        /**
         * @brief Read from the buffer.
         *
         * @param dst
         * @param block_offset
         * @param length
         */
        void host_read(void *dst, unsigned block_offset, unsigned length);

        /**
         * @brief Copy contents to another buffer.
         *
         * @param dst
         * @param regions
         * @param region_count
         */
        void copy_to(Buffer &dst, VkBufferCopy *regions, unsigned region_count);

        /**
         * @brief Destroy the buffer and free underlying memory.
         *
         */
        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan