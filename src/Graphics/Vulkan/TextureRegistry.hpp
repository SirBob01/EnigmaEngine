#pragma once

#include <unordered_map>

#include <vulkan/vulkan_core.h>

#include <Graphics/Texture.hpp>
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
        VkBorderColor border_color;
        unsigned mip_levels;

        bool operator==(const SamplerSettings &other) const {
            return u_address_mode == other.u_address_mode && v_address_mode == other.v_address_mode &&
                   w_address_mode == other.w_address_mode && min_filter == other.min_filter &&
                   mag_filter == other.mag_filter && border_color == other.border_color &&
                   mip_levels == other.mip_levels;
        }

        struct Hash {
            inline size_t operator()(const SamplerSettings &settings) const {
                size_t hash0 = std::hash<VkSamplerAddressMode>{}(settings.u_address_mode);
                size_t hash1 = std::hash<VkSamplerAddressMode>{}(settings.v_address_mode);
                size_t hash2 = std::hash<VkSamplerAddressMode>{}(settings.w_address_mode);
                size_t hash3 = std::hash<VkFilter>{}(settings.min_filter);
                size_t hash4 = std::hash<VkFilter>{}(settings.mag_filter);
                size_t hash5 = std::hash<VkBorderColor>{}(settings.border_color);
                size_t hash6 = std::hash<unsigned>{}(settings.mip_levels);

                return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4) ^ (hash5 << 5) ^ (hash6 << 6);
            }
        };
    };

    struct TextureInstance {
        VirtualImage image;
        VkImageView view;
        VkSampler sampler;
    };

    class TextureRegistry {
        VkDevice _device;
        const PhysicalDevice *_physical;

        VkQueue _transfer_queue;
        VkCommandBuffer _command_buffer;

        std::unordered_map<SamplerSettings, VkSampler, SamplerSettings::Hash> _samplers;
        SparseArray<Texture, TextureInstance> _instances;

        void write_texels(const TextureDescriptor &descriptor,
                          const VkImageSubresourceRange &aspect,
                          VkImage image,
                          MemoryPool &memory);

      public:
        TextureRegistry(VkDevice device, const PhysicalDevice &physical, VkCommandPool transfer_pool);
        TextureRegistry() = default;

        Texture build(const TextureDescriptor &descriptor, const Swapchain &swapchain, MemoryPool &memory);

        const TextureInstance &get(Texture texture) const;

        void destroy(Texture texture, MemoryPool &memory);

        void destroy(MemoryPool &memory);
    };
} // namespace Dynamo::Graphics::Vulkan