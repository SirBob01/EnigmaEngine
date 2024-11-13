#include <Graphics/Vulkan/RenderPassRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    RenderPassRegistry::RenderPassRegistry(VkDevice device,
                                           const PhysicalDevice &physical,
                                           const Swapchain &swapchain) :
        _device(device),
        _swapchain_color_format(swapchain.surface_format.format),
        _swapchain_depth_format(physical.depth_format),
        depth(build_depth_pass(physical.samples, _swapchain_depth_format)),
        shading(build_forward_pass(physical.samples, _swapchain_color_format, _swapchain_depth_format)) {}

    RenderPassRegistry::~RenderPassRegistry() {
        vkDestroyRenderPass(_device, depth.handle, nullptr);
        vkDestroyRenderPass(_device, shading.handle, nullptr);
    }

    RenderPassInstance RenderPassRegistry::build_depth_pass(VkSampleCountFlagBits samples,
                                                            VkFormat depth_stencil_format) {
        // Depth-stencil attachment and reference
        VkAttachmentDescription depth_stencil = {};
        depth_stencil.format = depth_stencil_format;
        depth_stencil.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_stencil.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_stencil.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_stencil.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_stencil.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_stencil.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_stencil.samples = samples;

        VkAttachmentReference depth_stencil_ref = {};
        depth_stencil_ref.attachment = 0;
        depth_stencil_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Subpass
        VkSubpassDescription subpass = {};
        subpass.pColorAttachments = nullptr;
        subpass.pResolveAttachments = nullptr;
        subpass.colorAttachmentCount = 0;
        subpass.pDepthStencilAttachment = &depth_stencil_ref;

        // Subpass dependency
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask =
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

        VkRenderPass handle = VkRenderPass_create(_device, &depth_stencil, 1, &subpass, 1, &dependency, 1);
        return {handle, samples};
    }

    RenderPassInstance RenderPassRegistry::build_forward_pass(VkSampleCountFlagBits samples,
                                                              VkFormat color_format,
                                                              VkFormat depth_stencil_format) {
        // Color attachment
        VkAttachmentDescription color = {};
        color.format = color_format;
        color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color.finalLayout = samples == VK_SAMPLE_COUNT_1_BIT ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                                                             : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color.samples = samples;

        VkAttachmentReference color_ref = {};
        color_ref.attachment = 0;
        color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Depth-stencil attachment
        VkAttachmentDescription depth_stencil = {};
        depth_stencil.format = depth_stencil_format;
        depth_stencil.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        depth_stencil.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_stencil.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_stencil.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_stencil.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_stencil.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        depth_stencil.samples = samples;

        VkAttachmentReference depth_stencil_ref = {};
        depth_stencil_ref.attachment = 1;
        depth_stencil_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

        // Color resolve attachment
        VkAttachmentDescription color_resolve = {};
        color_resolve.format = color_format;
        color_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        color_resolve.samples = VK_SAMPLE_COUNT_1_BIT;

        VkAttachmentReference color_resolve_ref = {};
        color_resolve_ref.attachment = 2;
        color_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_ref;
        subpass.pResolveAttachments = samples == VK_SAMPLE_COUNT_1_BIT ? nullptr : &color_resolve_ref;
        subpass.pDepthStencilAttachment = &depth_stencil_ref;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // If the sample count is 1, we ignore the color resolve attachment
        std::array<VkAttachmentDescription, 3> attachments = {
            color,
            depth_stencil,
            color_resolve,
        };
        VkRenderPass handle = VkRenderPass_create(_device,
                                                  attachments.data(),
                                                  samples == VK_SAMPLE_COUNT_1_BIT ? 2 : 3,
                                                  &subpass,
                                                  1,
                                                  &dependency,
                                                  1);
        return {handle, samples};
    }
} // namespace Dynamo::Graphics::Vulkan