#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    /**
     * @brief Maximum number of frames in flight.
     *
     */
    constexpr unsigned MAX_FRAMES_IN_FLIGHT = 3;

    /**
     * @brief Render frame context.
     *
     */
    struct FrameContext {
        VkFence sync_fence;
        VkSemaphore sync_render_start;
        VkSemaphore sync_render_done;
        VkCommandBuffer command_buffer;
    };

    /**
     * @brief Frame context list.
     *
     */
    class FrameContextList {
        VkDevice _device;
        std::array<FrameContext, MAX_FRAMES_IN_FLIGHT> _contexts;
        unsigned _index;

      public:
        /**
         * @brief Initialize the context objects.
         *
         * @param command_pool
         */
        FrameContextList(VkDevice device, VkCommandPool command_pool) : _device(device), _index(0) {
            std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> buffers;
            VkCommandBuffer_allocate(device,
                                     command_pool,
                                     VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                     buffers.data(),
                                     MAX_FRAMES_IN_FLIGHT);
            for (unsigned i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                _contexts[i].sync_fence = VkFence_create(device);
                _contexts[i].sync_render_start = VkSemaphore_create(device);
                _contexts[i].sync_render_done = VkSemaphore_create(device);
                _contexts[i].command_buffer = buffers[i];
            }
        }
        FrameContextList() = default;

        /**
         * @brief Grab the next context object.
         *
         * @return const FrameContext&
         */
        const FrameContext &next() {
            FrameContext &context = _contexts[_index];
            _index = (_index + 1) % MAX_FRAMES_IN_FLIGHT;
            return context;
        }

        /**
         * @brief Destroy the context objects.
         *
         */
        void destroy() {
            for (const FrameContext &context : _contexts) {
                vkDestroyFence(_device, context.sync_fence, nullptr);
                vkDestroySemaphore(_device, context.sync_render_start, nullptr);
                vkDestroySemaphore(_device, context.sync_render_done, nullptr);
            }
        }
    };
} // namespace Dynamo::Graphics::Vulkan