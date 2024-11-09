#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

namespace Dynamo::Graphics::Vulkan {

    struct QueueFamily {
        unsigned index = 0;
        unsigned count = 0;
        std::vector<float> priorities;
    };
    using QueueFamilyRef = std::reference_wrapper<const QueueFamily>;

    struct SwapchainOptions {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    struct PhysicalDevice {
        VkPhysicalDevice handle;
        VkSurfaceKHR surface;

        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceMaintenance3Properties maintenance;
        VkPhysicalDeviceMemoryProperties memory;
        VkPhysicalDeviceFeatures features;

        VkFormat depth_format;
        VkSampleCountFlagBits samples;

        QueueFamily graphics_queues;
        QueueFamily present_queues;
        QueueFamily compute_queues;
        QueueFamily transfer_queues;

        PhysicalDevice(VkPhysicalDevice handle, VkSurfaceKHR surface);
        PhysicalDevice() = default;

        static PhysicalDevice select_best(VkInstance instance, VkSurfaceKHR surface);

        SwapchainOptions get_swapchain_options() const;

        VkFormat get_supported_format(const VkFormat *candidates,
                                      unsigned count,
                                      VkImageTiling tiling,
                                      VkFormatFeatureFlags features) const;

        std::vector<QueueFamilyRef> unique_queue_families() const;

        std::vector<const char *> required_extensions() const;

        unsigned score() const;
    };
} // namespace Dynamo::Graphics::Vulkan