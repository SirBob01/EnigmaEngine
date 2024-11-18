#pragma once

#include <optional>

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Vulkan/Context.hpp>
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

        Swapchain(const Context &context, const Display &display, std::optional<Swapchain> previous = {});

        // Need to bypass destructor call order to perform swapchain recreation
        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan