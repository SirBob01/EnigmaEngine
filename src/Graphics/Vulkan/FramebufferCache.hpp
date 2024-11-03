#pragma once

#include <unordered_map>

#include <vulkan/vulkan_core.h>

namespace Dynamo::Graphics::Vulkan {
    struct FramebufferSettings {
        VkExtent2D extent;
        VkImageView color_view;
        VkImageView depth_stencil_view;
        VkImageView color_resolve_view;
        VkRenderPass renderpass;
        unsigned layers;

        inline bool operator==(const FramebufferSettings &other) const {
            return extent.width == other.extent.width && extent.height == other.extent.height &&
                   color_view == other.color_view && depth_stencil_view == other.depth_stencil_view &&
                   color_resolve_view == other.color_resolve_view && renderpass == other.renderpass &&
                   layers == other.layers;
        }

        struct Hash {
            inline size_t operator()(const FramebufferSettings &settings) const {
                size_t hash0 = std::hash<unsigned>{}(settings.extent.width);
                size_t hash1 = std::hash<unsigned>{}(settings.extent.height);
                size_t hash2 = std::hash<VkImageView>{}(settings.color_view);
                size_t hash3 = std::hash<VkImageView>{}(settings.depth_stencil_view);
                size_t hash4 = std::hash<VkImageView>{}(settings.color_resolve_view);
                size_t hash5 = std::hash<VkRenderPass>{}(settings.renderpass);
                size_t hash6 = std::hash<unsigned>{}(settings.layers);

                return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4) ^ (hash5 << 5) ^ (hash6 << 6);
            }
        };
    };

    class FramebufferCache {
        VkDevice _device;
        std::unordered_map<FramebufferSettings, VkFramebuffer, FramebufferSettings::Hash> _cache;

      public:
        FramebufferCache(VkDevice device);
        FramebufferCache() = default;

        VkFramebuffer get(const FramebufferSettings &settings);

        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan