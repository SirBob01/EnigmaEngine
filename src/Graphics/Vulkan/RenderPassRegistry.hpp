#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Pipeline.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct RenderPassInstance {
        VkRenderPass handle;
        VkSampleCountFlagBits samples;
    };

    class RenderPassRegistry {
        VkDevice _device;
        VkFormat _swapchain_color_format;
        VkFormat _swapchain_depth_format;

        RenderPassInstance
        build_forward_pass(VkSampleCountFlagBits samples, VkFormat color_format, VkFormat depth_stencil_format);

      public:
        const RenderPassInstance forward;

        RenderPassRegistry(VkDevice device, const PhysicalDevice &physical, const Swapchain &swapchain);
        ~RenderPassRegistry();
    };
} // namespace Dynamo::Graphics::Vulkan