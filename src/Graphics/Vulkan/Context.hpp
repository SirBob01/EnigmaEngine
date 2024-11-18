#pragma once

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct Context {
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugger;
        VkSurfaceKHR surface;

        PhysicalDevice physical;
        VkDevice device;

        VkCommandPool graphics_pool;
        VkCommandPool transfer_pool;

        VkQueue graphics_queue;
        VkQueue present_queue;
        VkQueue compute_queue;
        VkQueue transfer_queue;

        Context(const Display &display);
        ~Context();
    };
} // namespace Dynamo::Graphics::Vulkan