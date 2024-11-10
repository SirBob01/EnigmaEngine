#pragma once

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct VulkanContext {
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugger;
        VkSurfaceKHR surface;

        PhysicalDevice physical;
        VkDevice device;

        VkCommandPool graphics_pool;
        VkCommandPool transfer_pool;

        VulkanContext(const Display &display);
        ~VulkanContext();
    };
} // namespace Dynamo::Graphics::Vulkan