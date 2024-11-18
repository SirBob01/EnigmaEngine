#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    TextureRegistry::TextureRegistry(const Context &context, MemoryPool &memory) : _context(context), _memory(memory) {
        VkCommandBuffer_allocate(_context.device,
                                 _context.transfer_pool,
                                 VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                 &_command_buffer,
                                 1);
    }

    TextureRegistry::~TextureRegistry() {
        _instances.foreach ([&](TextureInstance &instance) {
            vkDestroyImageView(_context.device, instance.view, nullptr);
            _memory.free_image(instance.image);
        });
        _instances.clear();

        for (const auto &[key, sampler] : _samplers) {
            vkDestroySampler(_context.device, sampler, nullptr);
        }
        _samplers.clear();
    }

    void TextureRegistry::write_texels(const std::vector<unsigned char> &texels,
                                       VkImage image,
                                       VkFormat format,
                                       const VkExtent3D &extent,
                                       const VkImageSubresourceRange &subresources) {
        VirtualBuffer staging =
            _memory.allocate_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
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
        VkCommandBuffer_immediate_end(_command_buffer, _context.transfer_queue);

        // Free the staging buffer
        _memory.free_buffer(staging);
    }

    Texture TextureRegistry::build(const TextureDescriptor &descriptor, const Swapchain &swapchain) {
        TextureInstance instance;

        // Build sampler
        SamplerSettings sampler_settings;
        sampler_settings.u_address_mode = convert_texture_address_mode(descriptor.u_address_mode);
        sampler_settings.v_address_mode = convert_texture_address_mode(descriptor.v_address_mode);
        sampler_settings.w_address_mode = convert_texture_address_mode(descriptor.w_address_mode);
        sampler_settings.mag_filter = convert_texture_filter(descriptor.mag_filter);
        sampler_settings.min_filter = convert_texture_filter(descriptor.min_filter);
        sampler_settings.mipmap_mode = convert_texture_mipmap_filter(descriptor.mipmap_filter);
        sampler_settings.border_color = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        sampler_settings.mip_levels = descriptor.mip_levels;

        auto sampler_it = _samplers.find(sampler_settings);
        if (sampler_it != _samplers.end()) {
            instance.sampler = sampler_it->second;
        } else {
            instance.sampler = VkSampler_create(_context.device,
                                                sampler_settings.u_address_mode,
                                                sampler_settings.v_address_mode,
                                                sampler_settings.w_address_mode,
                                                sampler_settings.mag_filter,
                                                sampler_settings.min_filter,
                                                sampler_settings.mipmap_mode,
                                                sampler_settings.border_color,
                                                _context.physical.properties.limits.maxSamplerAnisotropy,
                                                sampler_settings.mip_levels);
            _samplers.emplace(sampler_settings, instance.sampler);
        }

        // Build the image
        VkExtent3D extent;
        extent.width = descriptor.width;
        extent.height = descriptor.height;
        extent.depth = descriptor.depth;

        VkImageSubresourceRange subresources;
        subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresources.baseArrayLayer = 0;
        subresources.layerCount = 1;
        subresources.baseMipLevel = 0;
        subresources.levelCount = descriptor.mip_levels;

        VkImageType type = descriptor.depth == 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        VkFormat format =
            convert_texture_format(descriptor.format, swapchain.surface_format, _context.physical.depth_format);
        VkSampleCountFlagBits samples =
            std::min(convert_texture_samples(descriptor.samples), _context.physical.samples);
        VkImageCreateFlags flags = 0;

        // Handle different usage cases
        switch (descriptor.usage) {
        case TextureUsage::Static:
            break;
        case TextureUsage::Cubemap:
            subresources.layerCount = 6;
            flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            break;
        case TextureUsage::ColorTarget:
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            break;
        case TextureUsage::DepthStencilTarget:
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            subresources.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            break;
        }

        instance.image = _memory.allocate_image(extent,
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
            write_texels(descriptor.texels, instance.image.image, format, extent, subresources);
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
        instance.view = VkImageView_create(_context.device, instance.image.image, format, view_type, subresources);

        return _instances.insert(instance);
    }

    const TextureInstance &TextureRegistry::get(Texture texture) const { return _instances.get(texture); }

    void TextureRegistry::destroy(Texture texture) {
        const TextureInstance &instance = _instances.get(texture);
        vkDestroyImageView(_context.device, instance.view, nullptr);
        _memory.free_image(instance.image);
        _instances.remove(texture);
    }
} // namespace Dynamo::Graphics::Vulkan