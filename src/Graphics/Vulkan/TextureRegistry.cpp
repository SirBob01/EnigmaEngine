#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    TextureRegistry::TextureRegistry(VkDevice device, const PhysicalDevice &physical, VkCommandPool transfer_pool) :
        _device(device), _physical(&physical) {
        VkCommandBuffer_allocate(_device, transfer_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &_command_buffer, 1);
        vkGetDeviceQueue(_device, physical.transfer_queues.index, 0, &_transfer_queue);
    }

    void TextureRegistry::write_texels(const std::vector<unsigned char> &texels,
                                       VkImage image,
                                       VkFormat format,
                                       const VkExtent3D &extent,
                                       const VkImageSubresourceRange &subresources,
                                       MemoryPool &memory) {
        VirtualBuffer staging = memory.build(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                             texels.size());
        std::memcpy(staging.mapped, texels.data(), texels.size());

        // Transition image to optimal layout for buffer copying
        VkCommandBuffer_immediate_start(_command_buffer);
        VkImage_transition_layout(image,
                                  _command_buffer,
                                  VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  subresources);

        // Copy buffer to image
        VkBufferImageCopy region = {};
        region.bufferOffset = staging.offset;
        region.imageSubresource.aspectMask = subresources.aspectMask;
        region.imageSubresource.baseArrayLayer = subresources.baseArrayLayer;
        region.imageSubresource.layerCount = subresources.layerCount;
        region.imageExtent = extent;

        unsigned stride = VkFormat_size(format);

        while (region.imageSubresource.mipLevel < subresources.levelCount) {
            // Copy buffer to image
            vkCmdCopyBufferToImage(_command_buffer,
                                   staging.buffer,
                                   image,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   1,
                                   &region);

            // Prepare for next LOD
            VkExtent3D &extent = region.imageExtent;
            region.bufferOffset += extent.width * extent.height * extent.depth * stride;
            extent.width = std::max(1U, extent.width / 2);
            extent.height = std::max(1U, extent.height / 2);
            extent.depth = std::max(1U, extent.depth / 2);
            region.imageSubresource.mipLevel++;
        }

        // Transition back to target layout
        VkImage_transition_layout(
            image,
            _command_buffer,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // TODO: should this depend on the texture usage?
            subresources);
        VkCommandBuffer_immediate_end(_command_buffer, _transfer_queue);

        // Free the staging buffer
        memory.free(staging);
    }

    Texture
    TextureRegistry::build(const TextureDescriptor &descriptor, const Swapchain &swapchain, MemoryPool &memory) {
        TextureInstance instance;

        // Build sampler
        SamplerSettings sampler_settings;
        sampler_settings.u_address_mode = convert_texture_address_mode(descriptor.u_address_mode);
        sampler_settings.v_address_mode = convert_texture_address_mode(descriptor.v_address_mode);
        sampler_settings.w_address_mode = convert_texture_address_mode(descriptor.w_address_mode);
        sampler_settings.mag_filter = convert_texture_filter(descriptor.mag_filter);
        sampler_settings.min_filter = convert_texture_filter(descriptor.min_filter);
        sampler_settings.border_color = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        sampler_settings.mip_levels = descriptor.mip_levels;

        auto sampler_it = _samplers.find(sampler_settings);
        if (sampler_it != _samplers.end()) {
            instance.sampler = sampler_it->second;
        } else {
            instance.sampler = VkSampler_create(_device,
                                                sampler_settings.u_address_mode,
                                                sampler_settings.v_address_mode,
                                                sampler_settings.w_address_mode,
                                                sampler_settings.mag_filter,
                                                sampler_settings.min_filter,
                                                sampler_settings.border_color,
                                                _physical->properties.limits.maxSamplerAnisotropy,
                                                sampler_settings.mip_levels);
            _samplers.emplace(sampler_settings, instance.sampler);
        }

        // Build the image
        VkExtent3D extent;
        extent.width = descriptor.width;
        extent.height = descriptor.height;
        extent.depth = descriptor.depth;

        VkImageSubresourceRange subresources;
        subresources.baseMipLevel = 0;
        subresources.baseArrayLayer = 0;
        subresources.levelCount = descriptor.mip_levels;

        VkImageType type = descriptor.depth == 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        VkFormat format;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
        VkImageCreateFlags flags = 0;

        // Handle different usage cases
        switch (descriptor.usage) {
        case TextureUsage::Static:
            subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresources.layerCount = 1;
            format = convert_texture_format(descriptor.format, swapchain.surface_format, _physical->depth_format);
            break;
        case TextureUsage::Cubemap:
            subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresources.layerCount = 6;
            format = convert_texture_format(descriptor.format, swapchain.surface_format, _physical->depth_format);
            flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            break;
        case TextureUsage::ColorTarget:
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresources.layerCount = 1;
            format = convert_texture_format(descriptor.format, swapchain.surface_format, _physical->depth_format);
            samples = _physical->msaa_samples;
            break;
        case TextureUsage::DepthStencilTarget:
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            subresources.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            subresources.layerCount = 1;
            format = convert_texture_format(descriptor.format, swapchain.surface_format, _physical->depth_format);
            samples = _physical->msaa_samples;
            break;
        }

        instance.image = memory.build(extent,
                                      format,
                                      VK_IMAGE_LAYOUT_UNDEFINED,
                                      type,
                                      tiling,
                                      usage,
                                      samples,
                                      flags,
                                      subresources.levelCount,
                                      subresources.layerCount);

        // Copy texels to staging buffer, if any
        if (descriptor.texels.size()) {
            write_texels(descriptor.texels, instance.image.image, format, extent, subresources, memory);
        }

        // Build image view
        VkImageViewType view_type;
        if (descriptor.usage == TextureUsage::Cubemap) {
            view_type = VK_IMAGE_VIEW_TYPE_CUBE;
        } else if (type == VK_IMAGE_TYPE_2D) {
            view_type = VK_IMAGE_VIEW_TYPE_2D;
        } else {
            view_type = VK_IMAGE_VIEW_TYPE_3D;
        }
        instance.view = VkImageView_create(_device, instance.image.image, format, view_type, subresources);

        return _instances.insert(instance);
    }

    const TextureInstance &TextureRegistry::get(Texture texture) const { return _instances.get(texture); }

    void TextureRegistry::destroy(Texture texture, MemoryPool &memory) {
        const TextureInstance &instance = _instances.get(texture);
        vkDestroyImageView(_device, instance.view, nullptr);
        memory.free(instance.image);
        _instances.remove(texture);
    }

    void TextureRegistry::destroy(MemoryPool &memory) {
        _instances.foreach ([&](TextureInstance &instance) {
            vkDestroyImageView(_device, instance.view, nullptr);
            memory.free(instance.image);
        });
        _instances.clear();

        for (const auto &[key, sampler] : _samplers) {
            vkDestroySampler(_device, sampler, nullptr);
        }
        _samplers.clear();
    }
} // namespace Dynamo::Graphics::Vulkan