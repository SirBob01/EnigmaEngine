#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    TextureRegistry::TextureRegistry(VkDevice device, const PhysicalDevice &physical, VkCommandPool transfer_pool) :
        _device(device), _physical(&physical) {
        VkCommandBuffer_allocate(_device, transfer_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &_command_buffer, 1);
        vkGetDeviceQueue(_device, physical.transfer_queues.index, 0, &_transfer_queue);
    }

    Texture TextureRegistry::build(const TextureDescriptor &descriptor, MemoryPool &memory) {
        TextureInstance instance;

        // Build sampler
        SamplerSettings sampler_settings;
        sampler_settings.u_address_mode = convert_texture_address_mode(descriptor.u_address_mode);
        sampler_settings.v_address_mode = convert_texture_address_mode(descriptor.v_address_mode);
        sampler_settings.w_address_mode = convert_texture_address_mode(descriptor.w_address_mode);
        sampler_settings.mag_filter = convert_texture_filter(descriptor.mag_filter);
        sampler_settings.min_filter = convert_texture_filter(descriptor.min_filter);
        sampler_settings.border_color = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

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
                                                _physical->properties.limits.maxSamplerAnisotropy);
            _samplers.emplace(sampler_settings, instance.sampler);
        }

        // Build image
        instance.image = memory.build(descriptor);

        // Copy texels to staging buffer
        VirtualBuffer staging = memory.build(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                             descriptor.texels.size());
        std::memcpy(staging.mapped, descriptor.texels.data(), descriptor.texels.size());

        VkImageSubresourceRange subresources;
        subresources.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresources.baseMipLevel = 0;
        subresources.levelCount = 1;
        subresources.baseArrayLayer = 0;
        subresources.layerCount = 1;

        // Transition image to optimal layout for buffer copying
        VkCommandBuffer_immediate_start(_command_buffer);
        VkImage_transition_layout(instance.image.image,
                                  _command_buffer,
                                  VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  subresources);

        // Copy buffer to image
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {descriptor.width, descriptor.height, 1};

        vkCmdCopyBufferToImage(_command_buffer,
                               staging.buffer,
                               instance.image.image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

        // Transition back to shader read optimal layout
        VkImage_transition_layout(instance.image.image,
                                  _command_buffer,
                                  VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  subresources);
        VkCommandBuffer_immediate_end(_command_buffer, _transfer_queue);

        // Free the staging buffer
        memory.free(staging);

        // Build image view
        instance.view = VkImageView_create(_device,
                                           instance.image.image,
                                           convert_texture_format(descriptor.format),
                                           VK_IMAGE_VIEW_TYPE_2D,
                                           subresources);

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