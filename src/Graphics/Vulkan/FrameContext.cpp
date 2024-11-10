#include <Graphics/Vulkan/FrameContext.hpp>

namespace Dynamo::Graphics::Vulkan {
    FrameContextList::FrameContextList(VkDevice device, VkCommandPool command_pool) : _device(device), _index(0) {
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

    FrameContextList::~FrameContextList() {
        for (const FrameContext &context : _contexts) {
            vkDestroyFence(_device, context.sync_fence, nullptr);
            vkDestroySemaphore(_device, context.sync_render_start, nullptr);
            vkDestroySemaphore(_device, context.sync_render_done, nullptr);
        }
    }

    const FrameContext &FrameContextList::next() {
        FrameContext &context = _contexts[_index];
        _index = (_index + 1) % MAX_FRAMES_IN_FLIGHT;
        return context;
    }
} // namespace Dynamo::Graphics::Vulkan