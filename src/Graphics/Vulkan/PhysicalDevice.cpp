#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Graphics::Vulkan {
    PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle, VkSurfaceKHR surface) : handle(handle), surface(surface) {
        maintenance.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES;
        maintenance.pNext = nullptr;

        VkPhysicalDeviceProperties2 properties2;
        properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        properties2.pNext = &maintenance;

        vkGetPhysicalDeviceProperties2(handle, &properties2);
        vkGetPhysicalDeviceMemoryProperties(handle, &memory);
        vkGetPhysicalDeviceFeatures(handle, &features);
        properties = properties2.properties;

        // Enumerate device queue families
        unsigned count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(count);
        vkGetPhysicalDeviceQueueFamilyProperties(handle, &count, queue_families.data());

        // Select queue families for each type
        unsigned index = 0;
        for (const VkQueueFamilyProperties family : queue_families) {
            VkBool32 surface_support = 0;
            VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(handle, index, surface, &surface_support);
            if (result != VK_SUCCESS) {
                Log::error("Vulkan::PhysicalDevice was unable to query for "
                           "surface support.");
            }

            // Dedicated presentation queues
            if (surface_support && family.queueCount > present_queues.count) {
                present_queues.count = family.queueCount;
                present_queues.index = index;
                present_queues.priorities.resize(family.queueCount, 0);
            }

            // Dedicated graphics queues
            if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && family.queueCount > graphics_queues.count) {
                graphics_queues.count = family.queueCount;
                graphics_queues.index = index;
                graphics_queues.priorities.resize(family.queueCount, 0);
            }

            // Dedicated transfer queues
            if ((family.queueFlags & VK_QUEUE_TRANSFER_BIT) && family.queueCount > transfer_queues.count) {
                transfer_queues.count = family.queueCount;
                transfer_queues.index = index;
                transfer_queues.priorities.resize(family.queueCount, 0);
            }

            // Dedicated compute queues
            if ((family.queueFlags & VK_QUEUE_COMPUTE_BIT) && family.queueCount > compute_queues.count) {
                compute_queues.count = family.queueCount;
                compute_queues.index = index;
                compute_queues.priorities.resize(family.queueCount, 0);
            }
        }
    }

    PhysicalDevice PhysicalDevice::select_best(VkInstance instance, VkSurfaceKHR surface) {
        unsigned count = 0;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        std::vector<VkPhysicalDevice> handles(count);
        vkEnumeratePhysicalDevices(instance, &count, handles.data());

        if (count == 0) {
            Log::error("No Vulkan-compatible physical devices available.");
        }

        Log::info("Selecting Vulkan device:");
        PhysicalDevice best = PhysicalDevice(handles[0], surface);
        for (unsigned i = 0; i < handles.size(); i++) {
            PhysicalDevice device = PhysicalDevice(handles[i], surface);
            unsigned score = device.score();
            if (score > best.score()) {
                best = device;
            }
            Log::info("* \"{}\" {} (Score {})",
                      device.properties.deviceName,
                      VkPhysicalDeviceType_string(device.properties.deviceType),
                      score);
        }

        if (best.score() == 0) {
            Log::error("No VkPhysicalDevice meets minimum requirements.");
        } else {
            Log::info("Vulkan using \"{}\"", best.properties.deviceName);
        }
        Log::info("");

        Log::info("Vulkan max allocation size: {}M", best.maintenance.maxMemoryAllocationSize / (1024.0 * 1024.0));
        Log::info("Vulkan max allocation count: {}", best.properties.limits.maxMemoryAllocationCount);
        Log::info("Vulkan max per-set descriptors: {}", best.maintenance.maxPerSetDescriptors);

        // Log device queue families
        Log::info("Vulkan graphics queues (Family Index: {} | Count: {})",
                  best.graphics_queues.index,
                  best.graphics_queues.count);
        Log::info("Vulkan present queues (Family Index: {} | Count: {})",
                  best.present_queues.index,
                  best.present_queues.count);
        Log::info("Vulkan transfer queues (Family Index: {} | Count: {})",
                  best.transfer_queues.index,
                  best.transfer_queues.count);
        Log::info("Vulkan compute queues (Family Index: {} | Count: {})",
                  best.compute_queues.index,
                  best.compute_queues.count);
        Log::info("");

        return best;
    }

    SwapchainOptions PhysicalDevice::get_swapchain_options() const {
        SwapchainOptions options;

        // Query for device surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(handle, surface, &options.capabilities);

        // Query for surface formats
        unsigned formats_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formats_count, nullptr);
        options.formats.resize(formats_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(handle, surface, &formats_count, options.formats.data());

        // Query for surface present modes
        unsigned present_modes_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &present_modes_count, nullptr);
        options.present_modes.resize(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(handle, surface, &present_modes_count, options.present_modes.data());

        return options;
    }

    std::vector<QueueFamilyRef> PhysicalDevice::unique_queue_families() const {
        std::array<QueueFamilyRef, 4> families = {
            graphics_queues,
            present_queues,
            transfer_queues,
            compute_queues,
        };

        std::vector<QueueFamilyRef> unique;
        for (const QueueFamily &family : families) {
            bool found = false;
            for (const QueueFamily &u_family : unique) {
                if (u_family.index == family.index) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                unique.push_back(family);
            }
        }
        return unique;
    }

    std::vector<const char *> PhysicalDevice::required_extensions() const {
        std::vector<const char *> required_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        // Check if portability subset extension is available
        unsigned count = 0;
        vkEnumerateDeviceExtensionProperties(handle, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateDeviceExtensionProperties(handle, nullptr, &count, extensions.data());

        const char *VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME = "VK_KHR_portability_subset";
        for (const VkExtensionProperties &extension : extensions) {
            if (!std::strcmp(extension.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) {
                required_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
            }
        }
        return required_extensions;
    }

    unsigned PhysicalDevice::score() const {
        SwapchainOptions swapchain_options = get_swapchain_options();
        if (
            // Required device features
            !features.fillModeNonSolid || !features.sampleRateShading || !features.samplerAnisotropy ||

            // Required device queues
            !graphics_queues.count || !transfer_queues.count || !present_queues.count || !compute_queues.count ||

            // Required swapchain support
            swapchain_options.present_modes.empty() || swapchain_options.formats.empty()) {
            return 0;
        }

        // Prioritize discrete GPU
        unsigned value = 0;
        switch (properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            value += 1000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            value += 500;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            value += 250;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            value += 100;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            value += 50;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
            break;
        }

        // Prioritize device with certain features
        value += features.geometryShader * 1000;
        value += features.tessellationShader * 1000;

        // Prioritize device with higher limits
        value += properties.limits.maxImageDimension2D;

        return value;
    }
} // namespace Dynamo::Graphics::Vulkan