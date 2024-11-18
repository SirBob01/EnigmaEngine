#include <Graphics/Vulkan/Swapchain.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    Swapchain::Swapchain(const Context &context, const Display &display, std::optional<Swapchain> previous) :
        device(context.device) {
        SwapchainOptions options = context.physical.get_swapchain_options();

        // Compute swapchain size
        Vec2 size = display.get_framebuffer_size();
        extent.width = std::clamp(static_cast<unsigned>(size.x),
                                  options.capabilities.minImageExtent.width,
                                  options.capabilities.maxImageExtent.width);
        extent.height = std::clamp(static_cast<unsigned>(size.y),
                                   options.capabilities.minImageExtent.height,
                                   options.capabilities.maxImageExtent.height);

        // Select optimal surface format
        surface_format = options.formats[0];
        for (VkSurfaceFormatKHR query : options.formats) {
            if (query.format == VK_FORMAT_B8G8R8A8_SRGB && query.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surface_format = query;
                break;
            }
        }

        // Select present mode based on display v-sync flag
        present_mode = VK_PRESENT_MODE_FIFO_KHR;
        for (VkPresentModeKHR query : options.present_modes) {
            if (!display.is_vsync() && query == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                present_mode = query;
                break;
            } else if (query == VK_PRESENT_MODE_MAILBOX_KHR) {
                present_mode = query;
                break;
            }
        }

        // Create swapchain handle
        VkSwapchainCreateInfoKHR swapchain_info = {};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = context.physical.surface;
        swapchain_info.preTransform = options.capabilities.currentTransform;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.presentMode = present_mode;
        swapchain_info.clipped = VK_TRUE;
        swapchain_info.imageFormat = surface_format.format;
        swapchain_info.imageColorSpace = surface_format.colorSpace;
        swapchain_info.imageExtent = extent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.minImageCount = options.capabilities.minImageCount + 1;
        if (options.capabilities.maxImageCount) {
            swapchain_info.minImageCount = options.capabilities.maxImageCount;
        }

        // Share images across graphics and present queue families
        std::array<unsigned, 2> queue_family_indices = {
            context.physical.graphics_queues.index,
            context.physical.present_queues.index,
        };
        if (queue_family_indices[0] != queue_family_indices[1]) {
            swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchain_info.queueFamilyIndexCount = queue_family_indices.size();
            swapchain_info.pQueueFamilyIndices = queue_family_indices.data();
        } else {
            swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchain_info.queueFamilyIndexCount = 0;
            swapchain_info.pQueueFamilyIndices = nullptr;
        }

        // Handle swapchain recreation
        swapchain_info.oldSwapchain = VK_NULL_HANDLE;
        if (previous.has_value()) {
            swapchain_info.oldSwapchain = previous.value().handle;
        }

        VkResult_check("Create Swapchain", vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &handle));

        // Destroy the old swapchain
        if (previous.has_value()) {
            previous.value().destroy();
        }

        // Get swapchain images
        unsigned count = 0;
        vkGetSwapchainImagesKHR(device, handle, &count, nullptr);
        images.resize(count);
        vkGetSwapchainImagesKHR(device, handle, &count, images.data());

        // Initialize swapchain views
        for (const VkImage image : images) {
            VkFormat format = surface_format.format;

            VkImageSubresourceRange subresources;
            subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresources.baseArrayLayer = 0;
            subresources.layerCount = 1;
            subresources.baseMipLevel = 0;
            subresources.levelCount = 1;

            VkImageView view = VkImageView_create(device, image, format, VK_IMAGE_VIEW_TYPE_2D, subresources);
            views.push_back(view);
        }

        // Number of array layers
        array_layers = swapchain_info.imageArrayLayers;
    }

    void Swapchain::destroy() {
        for (const VkImageView view : views) {
            vkDestroyImageView(device, view, nullptr);
        }
        vkDestroySwapchainKHR(device, handle, nullptr);
    }
} // namespace Dynamo::Graphics::Vulkan