#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    // Can't be too high or we'll experience latency
    constexpr unsigned MAX_FRAMES_IN_FLIGHT = 3;

    struct FrameContext {
        VkFence sync_fence;
        VkSemaphore sync_render_start;
        VkSemaphore sync_render_done;
        VkCommandBuffer command_buffer;
    };

    class FrameContextList {
        VkDevice _device;
        std::array<FrameContext, MAX_FRAMES_IN_FLIGHT> _contexts;
        unsigned _index;

      public:
        FrameContextList(VkDevice device, VkCommandPool command_pool);
        ~FrameContextList();

        const FrameContext &next();
    };
} // namespace Dynamo::Graphics::Vulkan