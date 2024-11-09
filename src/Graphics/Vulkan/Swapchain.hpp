#pragma once

#include <optional>

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct Swapchain {
        VkDevice device;
        VkSwapchainKHR handle;

        VkExtent2D extent;
        VkSurfaceFormatKHR surface_format;
        VkPresentModeKHR present_mode;

        std::vector<VkImage> images;
        std::vector<VkImageView> views;

        unsigned array_layers;

        Swapchain(VkDevice device,
                  const PhysicalDevice &physical,
                  const Display &display,
                  std::optional<Swapchain> previous = {});
        Swapchain() = default;

        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan