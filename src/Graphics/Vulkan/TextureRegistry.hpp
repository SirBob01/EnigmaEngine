#pragma once

#include <unordered_map>

#include <vulkan/vulkan_core.h>

#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/BufferRegistry.hpp>
#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct SamplerSettings {
        VkSamplerAddressMode u_address_mode;
        VkSamplerAddressMode v_address_mode;
        VkSamplerAddressMode w_address_mode;
        VkFilter min_filter;
        VkFilter mag_filter;
        VkSamplerMipmapMode mipmap_mode;
        VkBorderColor border_color;
        unsigned mip_levels;

        bool operator==(const SamplerSettings &other) const {
            return u_address_mode == other.u_address_mode && v_address_mode == other.v_address_mode &&
                   w_address_mode == other.w_address_mode && min_filter == other.min_filter &&
                   mag_filter == other.mag_filter && mipmap_mode == other.mipmap_mode &&
                   border_color == other.border_color && mip_levels == other.mip_levels;
        }

        struct Hash {
            inline size_t operator()(const SamplerSettings &settings) const {
                size_t hash0 = std::hash<VkSamplerAddressMode>{}(settings.u_address_mode);
                size_t hash1 = std::hash<VkSamplerAddressMode>{}(settings.v_address_mode);
                size_t hash2 = std::hash<VkSamplerAddressMode>{}(settings.w_address_mode);
                size_t hash3 = std::hash<VkFilter>{}(settings.min_filter);
                size_t hash4 = std::hash<VkFilter>{}(settings.mag_filter);
                size_t hash5 = std::hash<VkSamplerMipmapMode>{}(settings.mipmap_mode);
                size_t hash6 = std::hash<VkBorderColor>{}(settings.border_color);
                size_t hash7 = std::hash<unsigned>{}(settings.mip_levels);

                return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4) ^ (hash5 << 5) ^ (hash6 << 6) ^
                       (hash7 << 7);
            }
        };
    };

    struct TextureInstance {
        VkImage image;
        VkImageView view;
        VkSampler sampler;
        Allocation allocation;
    };

    class TextureRegistry {
        const Context &_context;
        MemoryPool &_memory;
        BufferRegistry &_buffers;

        std::unordered_map<SamplerSettings, VkSampler, SamplerSettings::Hash> _samplers;
        SparseArray<Texture, TextureInstance> _instances;

        void write_texels(const std::vector<unsigned char> &texels,
                          VkImage image,
                          VkFormat format,
                          const VkExtent3D &extent,
                          const VkImageSubresourceRange &subresources);

      public:
        TextureRegistry(const Context &context, MemoryPool &memory, BufferRegistry &buffers);
        ~TextureRegistry();

        Texture build(const TextureDescriptor &descriptor, const Swapchain &swapchain);

        const TextureInstance &get(Texture texture) const;

        void destroy(Texture texture);
    };
} // namespace Dynamo::Graphics::Vulkan